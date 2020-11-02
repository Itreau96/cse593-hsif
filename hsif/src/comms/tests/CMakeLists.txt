set(TARGET commsTests)

set(SOURCE
	utEndpoint.cpp
)

add_executable(${TARGET} ${SOURCE})

target_link_libraries(${TARGET} gtest gmock gtest_main comms)
add_subdirectory("${PROJECT_SOURCE_DIR}/googletest" "googletest")
gtest_discover_tests(${TARGET})