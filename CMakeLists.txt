cmake_minimum_required(VERSION 3.25)

project(imgui)
project(FogOfWarShared)
project(FogOfWarRenderer)
project(FogOfWar)

option(FOW_USE_CPP_20 "Use C++20 instead of C++23 when set to 'TRUE'" FALSE)

if (FOW_USE_CPP_20 OR WIN32)
    message(STATUS "Using C++20")
    set(CMAKE_CXX_STANDARD 20)
else()
    message(STATUS "Using C++23")
    set(CMAKE_CXX_STANDARD 23)
endif()

if (WIN32)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    include(vcpkg.cmake)
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(FOW_SCRIPTS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Scripts)
set(FOW_BUILD_DIRECTORY   ${CMAKE_CURRENT_LIST_DIR}/Build)
set(FOW_ENGINE_DIRECTORY  ${CMAKE_CURRENT_LIST_DIR}/Engine)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FOW_BUILD_DIRECTORY}/Bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${FOW_BUILD_DIRECTORY}/Bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${FOW_BUILD_DIRECTORY}/Lib/${CMAKE_BUILD_TYPE})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

macro(add_game TARGET_NAME)
    set(options "")
    set(oneValueArgs BASE_DIR RESOURCES_DIR SHADERS)
    set(multiValueArgs ASSETS SOURCES)

    cmake_parse_arguments(
        GAME
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if (NOT GAME_BASE_DIR)
        set(GAME_BASE_DIR ${TARGET_NAME})
    endif()
    if (NOT GAME_SOURCES)
        message(FATAL_ERROR "No sources defined")
    endif()

    if (NOT IS_ABSOLUTE ${GAME_BASE_DIR})
        cmake_path(ABSOLUTE_PATH GAME_BASE_DIR
            BASE_DIRECTORY ${FOW_BUILD_DIRECTORY}/Games
            NORMALIZE
            OUTPUT_VARIABLE GAME_BASE_DIR
        )
    endif()

    list(LENGTH GAME_ASSETS ASSET_COUNT)
    math(EXPR REMAINDER "${ASSET_COUNT} % 2")
    if(NOT REMAINDER EQUAL 0)
        message(FATAL_ERROR "ASSETS must be provided as (input output) pairs")
    endif()

    add_executable(${TARGET_NAME} ${GAME_SOURCES})
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${GAME_BASE_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${GAME_BASE_DIR}
        ARCHIVE_OUTPUT_DIRECTORY ${GAME_BASE_DIR}
    )
    target_link_libraries(${TARGET_NAME} PRIVATE FogOfWar::Engine)
    message(STATUS "Setting game base directory \"${GAME_BASE_DIR}\"")

    set(SHADERLIB_SRC ${CMAKE_CURRENT_LIST_DIR}/shaderlib/ShaderLib.cpp)
    set(SHADERLIB_INC ${CMAKE_CURRENT_LIST_DIR}/shaderlib ${FOW_ENGINE_DIRECTORY}/shaders/shaderlib)

    add_library(${TARGET_NAME}_ShaderLib SHARED ${SHADERLIB_SRC})
    target_compile_definitions(${TARGET_NAME}_ShaderLib PRIVATE -DSHADER_LIB_EXPORTS)
    target_include_directories(${TARGET_NAME}_ShaderLib PUBLIC ${SHADERLIB_INC})
    set_target_properties(${TARGET_NAME}_ShaderLib PROPERTIES
        OUTPUT_NAME shaderlib
        RUNTIME_OUTPUT_DIRECTORY ${GAME_BASE_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${GAME_BASE_DIR}
        ARCHIVE_OUTPUT_DIRECTORY ${GAME_BASE_DIR}
        ADDITIONAL_CLEAN_FILES   ${SHADERLIB_SRC}
    )
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}_ShaderLib)

    if (NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/shaderlib)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaderlib)
    endif()

    set(SHADER_COMPILE_ARGS
        --source-out "${CMAKE_CURRENT_LIST_DIR}/shaderlib"
        --engine-shaders "${FOW_ENGINE_DIRECTORY}/shaders"
    )

    if (GAME_SHADERS)
        set(SHADER_COMPILE_ARGS ${SHADER_COMPILE_ARGS} --game-shaders "${GAME_SHADERS}")
    endif()

    add_custom_target(${TARGET_NAME}_ShaderLibGenerate ALL DEPENDS ${SHADERLIB_SRC})
    add_custom_command(
        OUTPUT ${SHADERLIB_SRC}
        COMMAND "${Python3_EXECUTABLE}" "${FOW_SCRIPTS_DIRECTORY}/compile_shaders.py" ${SHADER_COMPILE_ARGS}
    )
    add_dependencies(${TARGET_NAME}_ShaderLib ${TARGET_NAME}_ShaderLibGenerate)

    if (NOT EXISTS ${GAME_BASE_DIR}/res)
        file(MAKE_DIRECTORY ${GAME_BASE_DIR}/res)
    endif()

    if (GAME_RESOURCES_DIR)
        add_custom_target(${TARGET_NAME}_CopyResources)
        add_custom_command(
            TARGET ${TARGET_NAME}_CopyResources
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${GAME_RESOURCES_DIR} ${GAME_BASE_DIR}/res
            VERBATIM
        )
        add_dependencies(${TARGET_NAME} ${TARGET_NAME}_CopyResources)
    endif()

    if (WIN32)
        add_custom_target(${TARGET_NAME}_CopyLibs)
        add_custom_command(
            TARGET ${TARGET_NAME}_CopyLibs
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/FogOfWarShared.dll
                    ${GAME_BASE_DIR}/FogOfWarShared.dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/FogOfWarRenderer.dll
                    ${GAME_BASE_DIR}/FogOfWarRenderer.dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/FogOfWarEngine.dll
                    ${GAME_BASE_DIR}/FogOfWarEngine.dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/imgui.dll
                    ${GAME_BASE_DIR}/imgui.dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/glfw3.dll
                    ${GAME_BASE_DIR}/glfw3.dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/glm.dll
                    ${GAME_BASE_DIR}/glm.dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/minizip.dll
                    ${GAME_BASE_DIR}/minizip.dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/poly2tri.dll
                    ${GAME_BASE_DIR}/poly2tri.dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/pugixml.dll
                    ${GAME_BASE_DIR}/pugixml.dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/zip.dll
                    ${GAME_BASE_DIR}/zip.dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/zlibd1.dll
                    ${GAME_BASE_DIR}/zlibd1.dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bz2d.dll
                    ${GAME_BASE_DIR}/bz2d.dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/assimp-vc143-mtd.dll # TODO: Search for the file instead of using a fixed name, to make sure copy works with different versions
                    ${GAME_BASE_DIR}/assimp-vc143-mtd.dll
            VERBATIM
        )
        add_dependencies(${TARGET_NAME}_CopyLibs FogOfWar::Engine)
        add_dependencies(${TARGET_NAME} ${TARGET_NAME}_CopyLibs)
    endif()

    math(EXPR PAIR_COUNT "${ASSET_COUNT} / 2")
    if (${PAIR_COUNT} GREATER 0)
        add_custom_target(${TARGET_NAME}_RebuildAssets)
        foreach(i RANGE 0 ${PAIR_COUNT} - 1)
            math(EXPR in_idx "2 * ${i}")
            math(EXPR out_idx "2 * ${i} + 1")

            list(GET GAME_ASSETS ${in_idx} INPUT)
            list(GET GAME_ASSETS ${out_idx} OUTPUT)

            if (NOT IS_ABSOLUTE ${INPUT})
                set(INPUT "${GAME_BASE_DIR}/${INPUT}")
            endif()
            if (NOT IS_ABSOLUTE ${OUTPUT})
                cmake_path(ABSOLUTE_PATH OUTPUT
                    BASE_DIRECTORY ${GAME_BASE_DIR}
                    NORMALIZE
                    OUTPUT_VARIABLE OUTPUT
                )
            endif()

            add_custom_command(
                TARGET ${TARGET_NAME}_RebuildAssets
                POST_BUILD
                COMMAND "${Python3_EXECUTABLE}"
                    "${FOW_SCRIPTS_DIRECTORY}/package_assets.py"
                    --input  "${INPUT}"
                    --output "${OUTPUT}"
                VERBATIM
            )
        endforeach()
        add_dependencies(${TARGET_NAME} ${TARGET_NAME}_RebuildAssets)
    else()
        message(WARNING "No game assets path specified!")
    endif()
endmacro()

add_subdirectory(ThirdParty)
add_subdirectory(Engine)
add_subdirectory(Tests)
add_subdirectory(Game)
add_subdirectory(Editor)