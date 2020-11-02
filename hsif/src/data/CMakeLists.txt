set (TARGET data)

set(SOURCE
	MsgData.cpp
	DataManager.cpp
)

set(HEADERS
	MsgData.hpp
	DataManager.hpp
)

add_library(${TARGET} STATIC
			${SOURCE}
			${HEADERS})

#target_include_directories( ${TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/src/comms )
#target_link_libraries(${TARGET} comms)

if (PACKAGE_TESTS)
	#add_subdirectory(tests)
endif()