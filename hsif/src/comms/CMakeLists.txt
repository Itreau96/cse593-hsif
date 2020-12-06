set(TARGET comms)

set(SOURCE
	Endpoint.cpp
	Server.cpp
)

set(HEADERS
	Endpoint.hpp
	Server.hpp
)

add_library(${TARGET} STATIC
			${SOURCE}
			${HEADERS})

target_include_directories( ${TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/src/data )
target_include_directories( ${TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/src/util )
target_link_libraries(${TARGET} data util )
target_include_directories(${TARGET} PUBLIC include)