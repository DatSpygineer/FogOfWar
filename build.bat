@echo off

set build_type="Release"
set clean_build=0

:parse_args
if %1 == "--help" (
    echo "Arguments:"
    echo "--help ==> Display this text"
    echo "--debug ==> Build with debug symbols"
    echo "--clean ==> Clear build folder before build"
    exit
)
else if %1 == "--debug" (
    set build_type="Debug"
)
else if %1 == "--clean" (
    set clean_build=1
)
shift
if not "%~1"=="" goto parse_args

if %clean_build (
    rd /s/q Build
)

mkdir Build
mkdir "Build/cmake-%build_type%"
cd "Build/cmake-%build_type%"

"cmake ../.. -DCMAKE_BUILD_TYPE=%build_type%"
cmake --build