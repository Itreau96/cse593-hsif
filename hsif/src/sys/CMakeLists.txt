set (TARGET sys)

file(GLOB SOURCE *.cpp)

file(GLOB HEADERS *.h)

add_library(${TARGET} STATIC
			${SOURCE}
			${HEADERS})

target_link_libraries(${TARGET} data comms sys)

if (PACKAGE_TESTS)
	add_subdirectory(tests)
endif()