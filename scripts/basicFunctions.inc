#!/bin/bash

set -e  # exit on error

# set up color commands
if [ -t 1 -a -t 2 -a \( `type -t tput` = "file" \) ]; then
  _colors=`tput colors`
  if [ "$_colors" -ge 256 ]; then
    INFO () { tput setaf 33;  /bin/echo "$@"; tput sgr0; }
    OK   () { tput setaf 28;  /bin/echo "$@"; tput sgr0; }
    WARN () { tput setaf 226; /bin/echo "$@"; tput sgr0; }
    ERR  () { tput setaf 196; /bin/echo "$@"; tput sgr0; }
  else
    INFO () { tput setaf 4; /bin/echo "$@"; tput sgr0; }
    OK   () { tput setaf 2; /bin/echo "$@"; tput sgr0; }
    WARN () { tput setaf 3; /bin/echo "$@"; tput sgr0; }
    ERR  () { tput setaf 1; /bin/echo "$@"; tput sgr0; }
  fi
else
  INFO () { /bin/echo "$@"; }
  OK   () { /bin/echo "$@"; }
  WARN () { /bin/echo "$@"; }
  ERR  () { /bin/echo "$@"; }
fi


function get_web_getter() {
  # get a getter
  if [ \! -z `type -t curl` ]; then
    GET="curl --silent --location --compressed -O"
  elif [ \! -z `type -t wget` ]; then
    GET="wget --quiet"
  else
    ERR "No getter (curl/wget)"
    exit 1
  fi
  OK Getter is \"$GET\"
}

function check_for_tools() {
  INFO -n 'Tools: '
  for tool in $@; do
    if [ -x `type -t $tool` ]; then
      WARN -n No $tool
    else
      INFO -n "$tool "
    fi
  done
  OK ok.
}

function load_submodule() {
  INFO Load "$1"
  git submodule init $1
  git submodule sync $1
  git submodule update $1
}

function check_for() {
  if [ ! -x `which $1` ]
  then
    ERR "$1 binary not found. $2"
    if [ "non-fatal" -ne "$3" ]
    then
      exit 1
    fi
  fi
}

pushd () {
    command pushd "$@" > /dev/null
}

popd () {
    command popd "$@" > /dev/null
}

function checkout() {
	DIR=$1
  REPO=$2
  BRANCH=$3
  TARGET_DIR=$4
  INFO "Installing or Updating branch $BRANCH from repository $REPO into $DIR/$TARGET_DIR"
  mkdir -p "$DIR"
  if [[ ! -d "$DIR/$TARGET_DIR" || ! -d "$DIR/$TARGET_DIR/.git" ]] 
  then
      pushd "$DIR"
      INFO Cloning git repository from "$REPO"
      git clone -b "$BRANCH" --depth=1 "$REPO" "$TARGET_DIR"
      load_submodule    	
    popd 
  else
    WARN $2 was already cloned, Updating
    pushd "$DIR/$TARGET_DIR"
      git pull origin "$BRANCH"
      load_submodule
    popd > /dev/null
  fi
  OK "Ok setting up $BRANCH"
}




