FIND_PACKAGE(GTest CONFIG REQUIRED)
FILE(GLOB RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(test_1 ${RESOURCES})
target_link_libraries(test_1 PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(AllTestsInMain test_1)


