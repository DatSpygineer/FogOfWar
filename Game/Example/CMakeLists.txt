add_game(FogOfWarGameExample
    ASSETS        ${CMAKE_CURRENT_LIST_DIR}/Assets Data.pak
    RESOURCES_DIR ${CMAKE_CURRENT_LIST_DIR}/res
    ENABLE_MODS TRUE
    SOURCES main.cpp Components.cpp
)