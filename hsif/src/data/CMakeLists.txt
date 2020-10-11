set (TARGET data)

set(SOURCE
	Message.cpp
)

set(HEADERS
	Message.hpp
)

add_library(${TARGET} STATIC
			${SOURCE}
			${HEADERS})

if (PACKAGE_TESTS)
	add_subdirectory(tests)
endif()