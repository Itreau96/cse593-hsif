
cmake_minimum_required( VERSION 3.17 )

project (HSIF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()

# Include sub-projects.
add_subdirectory(src)