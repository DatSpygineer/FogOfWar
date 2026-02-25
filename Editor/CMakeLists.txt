project(FogOfWarEditor)

set(CMAKE_AUTOMOC ON)

set(FOW_EDITOR_DIRECTORY ${FOW_BUILD_DIRECTORY}/Editor)

file(GLOB_RECURSE FOW_EDITOR_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.hpp ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL OpenGLWidgets)

qt_standard_project_setup()

qt_add_executable(FogOfWarEditor ${FOW_EDITOR_SOURCES})
target_link_libraries(FogOfWarEditor PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    FogOfWar::Renderer
)
target_include_directories(FogOfWarEditor PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
set_target_properties(FogOfWarEditor PROPERTIES
    WIN32_EXECUTABLE ON
    RUNTIME_OUTPUT_DIRECTORY ${FOW_EDITOR_DIRECTORY}
)
add_custom_command(TARGET FogOfWarEditor
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/res
        ${FOW_EDITOR_DIRECTORY}/Editor
)