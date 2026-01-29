file(GLOB FOW_SHARED_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Shared/*.cpp)
file(GLOB FOW_RENDERER_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Renderer/*.cpp ${CMAKE_CURRENT_LIST_DIR}/src/Renderer/glad/*.c)
file(GLOB FOW_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Engine/*.cpp)

find_package(OpenGL REQUIRED)

add_library(FogOfWarShared SHARED ${FOW_SHARED_SOURCES})
target_include_directories(FogOfWarShared PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_compile_definitions(FogOfWarShared PUBLIC -DGLM_ENABLE_EXPERIMENTAL -DGLM_FORCE_CXX20)
target_link_libraries(FogOfWarShared PUBLIC pugixml::pugixml reflectcpp::reflectcpp glm::glm libzip::zip nlohmann_json::nlohmann_json)
add_library(FogOfWar::Shared ALIAS FogOfWarShared)

add_library(FogOfWarRenderer SHARED ${FOW_RENDERER_SOURCES})
target_include_directories(FogOfWarRenderer PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(FogOfWarRenderer PUBLIC FogOfWar::Shared soil2 assimp::assimp freetype)
add_library(FogOfWar::Renderer ALIAS FogOfWarRenderer)

add_library(FogOfWarEngine SHARED
    ${FOW_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(FogOfWarEngine PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/imgui/backends
)
target_link_libraries(FogOfWarEngine PUBLIC FogOfWar::Shared FogOfWar::Renderer glfw)
target_compile_definitions(FogOfWarEngine PUBLIC -DGLFW_INCLUDE_NONE)
add_library(FogOfWar::Engine ALIAS FogOfWarEngine)