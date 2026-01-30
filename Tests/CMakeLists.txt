file(GLOB FOW_SHARED_TEST_SOURCES ${CMAKE_CURRENT_LIST_DIR}/Shared/*.cpp)
add_executable(FogOfWarSharedTest ${FOW_SHARED_TEST_SOURCES})
target_link_libraries(FogOfWarSharedTest PRIVATE FogOfWar::Shared GTest::gtest)

add_test(FogOfWarSharedTest FogOfWarSharedTest)