if (NOT WIN32) # VCPKG version of assimp already links pugixml
    add_subdirectory(pugixml)
endif()

add_subdirectory(reflect-cpp)
add_subdirectory(nlohmann_json)
add_subdirectory(glm)

set(SOIL2_BUILD_TESTS OFF)
add_subdirectory(SOIL2)
