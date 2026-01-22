#! /bin/bash

build_type="Release"

if [[ "$#" -ne 0 ]]; then
  if [[ "$0" == "--debug" ]]; then
    build_type="Debug"
  fi
  if [[ "$0" == "--help" ]]; then
    echo "Arguments:"
    echo "--help ==> Display this text"
    echo "--debug ==> Build with debug symbols"
    exit 0
  fi
fi

mkdir -p Build/cmake-${build_type}
cd Build/cmake-${build_type} || goto cd_fail
cmake ../.. -DCMAKE_BUILD_TYPE=${build_type}
make all
exit 0

cd_fail:
echo "Failed to enter build folder \"Build/cmake-${build_type}\""
exit 1