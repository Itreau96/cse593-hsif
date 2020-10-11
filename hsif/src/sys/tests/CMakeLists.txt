set(TARGET sysTests)

file(GLOB SOURCE *.cpp)

add_executable(${TARGET} ${SOURCE})
target_link_libraries(${TARGET} gtest gmock gtest_main sys)
add_subdirectory("${PROJECT_SOURCE_DIR}/googletest" "googletest")
gtest_discover_tests(${TARGET})