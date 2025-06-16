#!/bin/sh
set -e
cd $(dirname "$0")
clang-format -style=Google -i src/*.cc src/include/*.h tests/*.cc
