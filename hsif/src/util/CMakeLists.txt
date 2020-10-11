set(TARGET util)

file(GLOB HEADERS *.h)

add_library(${TARGET} INTERFACE)
target_sources(${TARGET} INTERFACE ${HEADERS})

if (PACKAGE_TESTS)
	add_subdirectory(tests)
endif()