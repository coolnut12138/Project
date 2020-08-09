# cmake_minimum_required(VERSION 2.8.10.2)
# project(myleptjson VERSION 0.1.0)

# include(CTest)
# enable_testing()

# add_library(myleptjson myleptjson.c)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
cmake_minimum_required (VERSION 2.8.10.2)
project (myleptjson_test C)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall")
endif()

add_library(myleptjson myleptjson.c)
add_executable(myleptjson_test test.c)
target_link_libraries(myleptjson_test myleptjson)