#! /bin/bash

build_type="Release"
clean_build=false

if [[ "$#" -ne 0 ]]; then
  while [[ $# -gt 0 ]]; do
    if [[ "$1" == "--debug" ]]; then
      build_type="Debug"
    fi
    if [[ "$1" == "--clean" ]]; then
      clean_build=true
    fi
    if [[ "$1" == "--cc" ]]; then
      export CC="$2"
      shift
    fi
    if [[ "$1" == "--cxx" ]]; then
      export CXX="$2"
      shift
    fi
    if [[ "$1" == "--help" ]]; then
      echo "Arguments:"
      echo "--help ==> Display this text"
      echo "--debug ==> Build with debug symbols"
      echo "--clean ==> Clear build folder before build"
      echo "--cc <value> ==> Set C compiler"
      echo "--cxx <value> ==> Set C++ compiler"
      exit 0
    fi
    shift
  done
fi

if [[ "$clean_build" == true ]]; then
  rm -rf Build
fi

mkdir -p Build/cmake-${build_type}
cd Build/cmake-${build_type} || goto cd_fail
cmake ../.. -DCMAKE_BUILD_TYPE=${build_type}
cmake --build -j${nproc}
exit 0

cd_fail:
echo "Failed to enter build folder \"Build/cmake-${build_type}\""
exit 1