set (TARGET sys)

set(SOURCE
	Sys.cpp
)

set(HEADERS
	Sys.hpp
)

add_library(${TARGET} STATIC
			${SOURCE}
			${HEADERS})

target_include_directories( ${TARGET} 
	PUBLIC 
	${CMAKE_SOURCE_DIR}/src/data
	${CMAKE_SOURCE_DIR}/src/comms
	${CMAKE_SOURCE_DIR}/src/util
)

target_link_libraries(${TARGET} data comms util )
target_include_directories(${TARGET} PUBLIC include)