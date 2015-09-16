#!/bin/bash

#
# This is a setup script to get llvm version 3.3 up and running on
#   your system for use with this repo.
#

  ##############
  #  OS Check  #
  ##############

case "$(uname)" in
  Darwin) ;;
  *) echo "Platform other than Mac not supported at this time." >&2; exit 1;;
esac

  ###############
  #  ENV Setup  #
  ###############

TERM_WIDTH=`tput cols`
PROG='setup_llvm'
REPO=$( cd "$(dirname "$0")/.."; pwd -P )

  ####################
  # Shared Functions #
  ####################

# these methods used to simplify filepaths
cd_repo() {
  cd $REPO
}

cd_llvm() {
  cd $REPO/llvm
}

# loggers
log() {
  local msg=$1

  printf "\n%b\n" "${msg}" | fold -sw $TERM_WIDTH
}

log_stderr() {
  >&2 log "$1"
}

# exit functions
exit_routine() {
  local exit_status=$1
  local lineno=$2

  if ! [ "${exit_status}" = "0" ]; then
    if [ "${exit_status}" = "127" ]; then
      log_stderr "$PROG: Ensure you have the command installed and in your \$PATH."
    else
      log_stderr "$PROG: Error: Line ${lineno}"
    fi
  fi

  exit ${exit_status}
}

trap 'exit_routine $? $LINENO' EXIT

  ###########
  #  Setup  #
  ###########

# script will immediately exit anytime there is a failure
set -e

cd_repo

  ##############
  #  Get LLVM  #
  ##############

log 'Getting llvm. Extracting to mock-c-compiler/llvm ...'
  if ! [ -d $REPO/llvm ]; then
    mkdir $REPO/llvm/
  else
    rm -rf $REPO/llvm/*
  fi

  cd_llvm
  curl -O "http://llvm.org/releases/3.5.0/llvm-3.5.0.src.tar.xz"

  ################
  #  Upack LLVM  #
  ################

log 'Uncompressing llvm source ...'
  if gunzip llvm-3.5.0.src.tar.xz --test >/dev/null; then
    gunzip llvm-3.5.0.src.tar.xz --name 2>> tar_log
  fi

log 'Unpacking llvm source ...'
  if [ -s llvm-3.5.0.src.tar ]; then
    tar -xvzf llvm-3.5.0.src.tar 2>> tar_log
  fi

  ###########################
  #  Make and Install LLVM  #
  ###########################

log 'Creating Unix Makefile for llvm build ...'
  mkdir -p $REPO/llvm/build && cd $REPO/llvm/build

  cmake -G "Unix Makefiles" ../llvm-3.5.0.src/
  make
  sudo make install

log '--- llvm setup complete! ---"

exit 0 # Success
