set(VCPKG_ROOT $ENV{VCPKG_ROOT})

if (NOT VCPKG_ROOT)
    if (WIN32)
        find_program(VCPKG_PATH NAMES vcpkg.exe)
    else()
        find_program(VCPKG_PATH NAMES vcpkg)
    endif()

    get_filename_component(VCPKG_BASE_PATH ${VCPKG_PATH} DIRECTORY)
else()
    set(VCPKG_BASE_PATH ${VCPKG_ROOT})
endif()

include(${VCPKG_BASE_PATH}/scripts/buildsystems/vcpkg.cmake)