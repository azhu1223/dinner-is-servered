#!/bin/sh

# Place in dinner-is-servered

BUILD_ARGS="test"

if [ "$#" -gt 0 ]; then
    if [ "$#" -gt 1 ] || [ "$1" = "--help" ]; then
        echo "Usage: ./build-all.sh [--full/--help]"
        exit 0
    elif [ "$1" = "--full" ]; then
        BUILD_ARGS="--full"
    fi
fi

./build-server.sh "$BUILD_ARGS"
./build-coverage.sh "$BUILD_ARGS"
