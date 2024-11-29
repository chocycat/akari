#!/bin/bash

set -e 

BUILD_DIR="build"

if [ ! -d "$BUILD_DIR" ]; then
  echo "Setup"
  echo "-----"

  meson setup "$BUILD_DIR"
fi

echo

echo "Build"
echo "-----"

ninja -C "$BUILD_DIR"

echo
