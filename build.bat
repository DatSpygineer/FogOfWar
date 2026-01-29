@ECHO OFF

set buildType="Release"
set clean=0

for %%a in (%*) do (
    if %%a=="--help" (
        echo "Arguments:"
        echo "--help ==> Display this text"
        echo "--debug ==> Build with debug symbols"
        echo "--clean ==> Clear build folder before build"
        exit
    )
    if %%a=="--debug" (
        set buildType="Debug"
    )
    if %%a=="--clean" (
        set clean=1
    )
)

if %clean%==0 (
    rmdir /s /q Build
)

if not exist "Build" (
    mkdir "Build"
)
cd "Build"

if not exist "%buildType%" (
    mkdir "%buildType%"
)
cd "%buildType%"

cmake ../.. -DCMAKE_BUILD_TYPE=%buildType%
cmake --build