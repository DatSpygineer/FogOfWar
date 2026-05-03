add_game(FogOfWarGameExample
    ASSETS        ${CMAKE_CURRENT_LIST_DIR}/Assets Data.pak
    RESOURCES_DIR ${CMAKE_CURRENT_LIST_DIR}/res
    ENABLE_MODS   1
    ENABLE_EDITOR 1
    SOURCES main.cpp Components.cpp
)