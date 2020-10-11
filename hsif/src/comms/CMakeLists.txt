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

target_link_libraries(${TARGET} data)

if (PACKAGE_TESTS)
	add_subdirectory(tests)
endif()