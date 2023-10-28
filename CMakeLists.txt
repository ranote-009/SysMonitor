cmake_minimum_required(VERSION 3.22)

project(ClientServerGO)

# Find and configure Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS) 

add_subdirectory(Server)
add_subdirectory(Client)


