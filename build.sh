#!/bin/sh

# Place in dinner-is-servered and run with ./build.sh
rm -rf build/ build_coverage/
mkdir build/ build_coverage/
(cd build && cmake .. && make && make test)
(cd build_coverage && cmake -DCMAKE_BUILD_TYPE=Coverage .. && make coverage)