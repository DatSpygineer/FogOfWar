if (NOT WIN32) # VCPKG version of assimp already links pugixml
    add_subdirectory(pugixml)
endif()

add_subdirectory(reflect-cpp)
add_subdirectory(nlohmann_json)
add_subdirectory(glm)

set(SOIL2_BUILD_TESTS OFF)
add_subdirectory(SOIL2)

if (WIN32)
    set(MSDFGEN_USE_VCPKG ON)
    set(MSDF_ATLAS_USE_VCPKG ON)
else()
    set(MSDFGEN_USE_VCPKG OFF)
    set(MSDF_ATLAS_USE_VCPKG OFF)
endif()

set(MSDFGEN_USE_SKIA OFF)
set(MSDF_ATLAS_USE_SKIA OFF)
add_subdirectory(msdf-atlas-gen)

add_subdirectory(lua)
add_subdirectory(sol2)
