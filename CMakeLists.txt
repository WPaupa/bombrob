project(BOMBOWE_ROBOTY)
cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_COMPILER g++-10)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -Wconversion -O2)

file(GLOB client_src CONFIGURE_DEPENDS "client/*.h" "client/*.cpp" "common/*.h" "common/*.cpp")
file(GLOB server_src CONFIGURE_DEPENDS "server/*.h" "server/*.cpp" "common/*.h" "common/*.cpp")

add_executable(client ${client_src})
add_executable(server ${server_src})

find_package(Boost COMPONENTS program_options system thread REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(client ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(server ${Boost_LIBRARIES})