cmake_minimum_required(VERSION 3.22) 
set(CMAKE_CXX_STANDARD 17)
project(server) 

find_package(Boost COMPONENTS ALL REQUIRED) 

#This line because you are using some depericated Boost APIs 
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS) 

include_directories(${Boost_INCLUDE_DIRS}) 
add_executable(${PROJECT_NAME} projectServer.cpp) 
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES}) 
target_link_libraries(${PROJECT_NAME} mysqlcppconn) 


# add_executable(server projectServer.cpp)

# # target_link_libraries(server
# #     PRIVATE
# #     Boost::system
# #     Boost::filesystem
# #     Boost::beast
# #     mysqlcppconn)
# # target_link_libraries(server ${Boost_Lib}
# #     mysqlcppconn)

#     # include_directories(${Boost_INCLUDE_DIRS}) 
# # add_executable(${PROJECT_NAME} projectServer.cpp) 
# target_link_libraries(server ${Boost_LIBRARIES}) 
# target_link_libraries(server mysqlcppconn)