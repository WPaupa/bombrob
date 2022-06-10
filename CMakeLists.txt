project(BOMBOWE_ROBOTY)
cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_COMPILER g++-10)
add_compile_options(-Wall -Wextra -Wconversion -Werror -O2 -std=gnu++20)

file(GLOB client_src CONFIGURE_DEPENDS "client/*.h" "client/*.cpp" "common/*.h" "common/*.cpp")
file(GLOB server_src CONFIGURE_DEPENDS "server/*.h" "server/*.cpp" "common/*.h" "common/*.cpp")

add_executable(robots-client ${client_src})
add_executable(robots-server ${server_src})

find_package(Boost COMPONENTS program_options system thread chrono REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(robots-client ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(robots-server ${Boost_LIBRARIES})