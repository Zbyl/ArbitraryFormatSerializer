#!/usr/bin/env bash

#this script assumes, that it's first parameter is Boost minor version
#this script will export BOOST_ROOT variable pointing to Boost root directory

BOOST_VER="$1"
USE_GIT="0"

if [ "$BOOST_VER" = "" ]; then
    echo "Please provide Boost minor version as the first parameter."
    exit 1
fi

# quit on errors
set -e

pushd `pwd`

# Articles:
# downloading boost from sourceforge: https://github.com/tplgy/bonefish/blob/master/.travis.yml
# cloning boost from github: https://github.com/mat007/turtle/blob/4c5f9c321849ea7392bc85bf47202e669fcdfd01/.travis.yml

CI_ROOT="$HOME/ci-build-tmp"
export BOOST_ROOT="$CI_ROOT/boost"

if [ "$USE_GIT" = "0" ]; then
    echo "Downloading boost 1.$BOOST_VER from sourceforge."

    BOOST_DOWNLOAD_URL="http://sourceforge.net/projects/boost/files/boost/1.$BOOST_VER.0/boost_1_${BOOST_VER}_0.tar.bz2/download"
    DOWNLOAD_ROOT="$CI_ROOT/download"
    mkdir -p "$DOWNLOAD_ROOT"
    mkdir -p "$BOOST_ROOT"

    wget --no-verbose --progress=dot --output-document="$DOWNLOAD_ROOT/boost.tar.bz2" "$BOOST_DOWNLOAD_URL"

    echo "Extracting Boost."
    tar jxf "$DOWNLOAD_ROOT/boost.tar.bz2" --strip-components=1 -C "$BOOST_ROOT"

    #cd "$BOOST_ROOT"
    #BOOST_TOOLSET=clang
    #./bootstrap.sh --with-toolset="$BOOST_TOOLSET" --with-libraries=program_options,system,thread,regex
else
    echo "Cloning boost 1.$BOOST_VER from github."

    BOOST_BRANCH="boost-1.$BOOST_VER.0"
    mkdir -p `dirname "$BOOST_ROOT"`
    git --version
    git clone -b $BOOST_BRANCH https://github.com/boostorg/boost.git $BOOST_ROOT
    cd $BOOST_ROOT
    git submodule update --init --merge

    echo "Preparing Boost headers."
    ./bootstrap.sh
    ./b2 headers
fi

popd

echo "Boost succesfully set up in $BOOST_ROOT"
