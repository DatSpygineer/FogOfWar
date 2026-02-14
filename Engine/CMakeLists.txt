file(GLOB FOW_SHARED_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Shared/*.cpp)
file(GLOB FOW_RENDERER_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Renderer/*.cpp)
file(GLOB FOW_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Engine/*.cpp)
file(GLOB FOW_GLAD_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Renderer/glad/*.c)

if (WIN32)
    set(FOW_SHARED_OS_SPEC_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Shared/OsSpecific/Windows.cpp)
else()
    set(FOW_SHARED_OS_SPEC_SOURCES )
endif()

find_package(libzip CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)
find_package(pugixml CONFIG REQUIRED)

set(FOW_IMGUI_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui SHARED ${FOW_IMGUI_SOURCES})
target_compile_definitions(imgui PUBLIC "-DIMGUI_USER_CONFIG=\"fow/Engine/ImGuiCfg.hpp\"")
target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/backends
)
target_link_libraries(imgui PUBLIC OpenGL::GL glfw)

add_library(FogOfWarShared SHARED ${FOW_SHARED_SOURCES} ${FOW_SHARED_OS_SPEC_SOURCES})
target_include_directories(FogOfWarShared PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_compile_definitions(FogOfWarShared PUBLIC -DGLM_ENABLE_EXPERIMENTAL -DGLM_FORCE_CXX20)
target_link_libraries(FogOfWarShared PUBLIC pugixml::pugixml reflectcpp::reflectcpp glm::glm libzip::zip nlohmann_json::nlohmann_json)
add_library(FogOfWar::Shared ALIAS FogOfWarShared)

if (WIN32)
    target_link_libraries(FogOfWarShared PUBLIC Shlwapi.lib)
endif()

add_library(FogOfWarRenderer SHARED ${FOW_RENDERER_SOURCES} ${FOW_GLAD_SOURCES})
target_include_directories(FogOfWarRenderer PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${ASSIMP_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(FogOfWarRenderer PUBLIC FogOfWar::Shared soil2 ${ASSIMP_LIBRARIES} ${FREETYPE_LIBRARIES})
add_library(FogOfWar::Renderer ALIAS FogOfWarRenderer)

add_library(FogOfWarEngine SHARED ${FOW_SOURCES})
target_include_directories(FogOfWarEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(FogOfWarEngine PUBLIC FogOfWar::Shared FogOfWar::Renderer glfw imgui)
target_compile_definitions(FogOfWarEngine PUBLIC -DGLFW_INCLUDE_NONE)
add_library(FogOfWar::Engine ALIAS FogOfWarEngine)