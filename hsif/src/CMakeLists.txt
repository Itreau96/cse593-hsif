add_executable(HSIF main.cpp)

add_subdirectory(data)
add_subdirectory(comms)
add_subdirectory(util)
add_subdirectory(sys)

target_link_libraries(HSIF sys )