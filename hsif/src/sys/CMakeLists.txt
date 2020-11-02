set (TARGET sys)

file(GLOB SOURCE *.cpp)

file(GLOB HEADERS *.h)

add_library(${TARGET} STATIC
			${SOURCE}
			${HEADERS})

target_include_directories( ${TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/src/data )
target_include_directories( ${TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/src/comms )
target_link_libraries(${TARGET} data comms )

if (PACKAGE_TESTS)
	#add_subdirectory(tests)
endif()