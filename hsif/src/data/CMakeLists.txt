set (TARGET data)

set(SOURCE
	MsgData.cpp
)

set(HEADERS
	MsgData.hpp
)

add_library(${TARGET} STATIC
			${SOURCE}
			${HEADERS})

target_link_libraries(${TARGET} nlohmann_json::nlohmann_json)
target_include_directories(${TARGET} PUBLIC include)