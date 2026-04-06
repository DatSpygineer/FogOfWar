file(GLOB FOW_SHARED_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Shared/*.cpp)
file(GLOB FOW_RENDERER_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Renderer/*.cpp)
file(GLOB FOW_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Engine/*.cpp)

if (WIN32)
    set(FOW_SHARED_OS_SPEC_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Shared/OsSpecific/Windows.cpp)
else()
    set(FOW_SHARED_OS_SPEC_SOURCES )
endif()

find_package(libzip CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(assimp REQUIRED)

if (WIN32)
    find_package(pugixml CONFIG REQUIRED) # On Linux Assimp already loads pugixml already
    find_package(GLEW CONFIG REQUIRED)
    set(FOW_GLAD_SOURCES )
else()
    file(GLOB FOW_GLAD_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Renderer/glad/*.c)
endif()

set(FOW_IMGUI_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/backends/imgui_impl_sdl3.cpp
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
target_link_libraries(FogOfWarShared PUBLIC
    pugixml::pugixml
    reflectcpp::reflectcpp
    glm::glm
    libzip::zip
    nlohmann_json::nlohmann_json
    SDL3::SDL3
)
add_library(FogOfWar::Shared ALIAS FogOfWarShared)

if (WIN32)
    target_link_libraries(FogOfWarShared PUBLIC Shlwapi.lib)
endif()

add_library(FogOfWarRenderer SHARED ${FOW_RENDERER_SOURCES} ${FOW_GLAD_SOURCES})
target_include_directories(FogOfWarRenderer PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(FogOfWarRenderer PUBLIC
    FogOfWar::Shared
    SDL3_ttf::SDL3_ttf
    soil2
    ${ASSIMP_LIBRARIES}
)
add_library(FogOfWar::Renderer ALIAS FogOfWarRenderer)
if (WIN32)
    target_link_libraries(FogOfWarRenderer PUBLIC GLEW::GLEW)
endif()

add_library(FogOfWarEngine SHARED ${FOW_SOURCES})
target_include_directories(FogOfWarEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(FogOfWarEngine PUBLIC FogOfWar::Shared FogOfWar::Renderer imgui)
target_compile_definitions(FogOfWarEngine PUBLIC -DGLFW_INCLUDE_NONE)
add_library(FogOfWar::Engine ALIAS FogOfWarEngine)