#!/usr/bin/env bash

# quit on errors
set -e

pushd `pwd`

echo "Testing..."
./build-zz/Example-Person
echo "Testing done."

popd
