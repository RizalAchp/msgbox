#!/usr/bin/env bash

export CC=$(which clang)
export CXX=$(which clang++)
PROJECT_ROOT="$(pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
BUILD_TYPE="${1:-Debug}"

exiting() {
    unset CC
    unset CXX
    exit 0
}

main() {
    cmake -B ${BUILD_DIR} -S ${PROJECT_ROOT}
    exiting
}

main
