set( TARGET util )

set( HEADERS
	Logger.hpp
)

add_library(${TARGET} INTERFACE)
target_sources(${TARGET} INTERFACE ${HEADERS})