#!/bin/sh

# Place in dinner-is-servered

if [ "$#" -gt 0 ]; then
    if [ "$#" -gt 1 ] || [ "$1" = "--help" ]; then
        echo "Usage: ./build-coverage.sh [--full/--help]"
        exit 0
    elif [ "$1" = "--full" ]; then
        rm -rf build_coverage/*
    fi
fi

(cd build_coverage && cmake -DCMAKE_BUILD_TYPE=Coverage .. && make coverage)
