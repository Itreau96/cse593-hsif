set(TARGET hsifTests)

set(SOURCE
	utLogger.cpp
	utMsgData.cpp
	utEndpoint.cpp
	utServer.cpp
)


add_executable(${TARGET} ${SOURCE})
target_link_libraries(${TARGET} gtest gmock gtest_main util data comms)
add_subdirectory("${PROJECT_SOURCE_DIR}/googletest" "googletest")
target_include_directories(${TARGET} 
	PUBLIC 
	${CMAKE_SOURCE_DIR}/src/util
	${CMAKE_SOURCE_DIR}/src/data
	${CMAKE_SOURCE_DIR}/src/comms
)
gtest_discover_tests(${TARGET})