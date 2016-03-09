#!/usr/bin/env bash

# quit on errors
set -e

pushd `pwd`

echo "Testing..."
./Example-Person
echo "Testing done."

popd
