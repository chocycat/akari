#!/bin/bash

set -e 

BUILD_DIR="build"

if [ ! -d "$BUILD_DIR" ]; then
  echo "Setup"
  echo "-----"

  meson setup "$BUILD_DIR" -Dexamples=true
fi

echo

echo "Build"
echo "-----"

ninja -C "$BUILD_DIR"

echo
