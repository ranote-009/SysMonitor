cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)  

project(ClientServerGO)

# Find and configure Boost
#set(CMAKE_PREFIX_PATH "/usr/include/boost/beast")


find_package(Boost REQUIRED COMPONENTS system filesystem beast)

add_subdirectory(Server)
add_subdirectory(Client)


