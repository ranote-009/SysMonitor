cmake_minimum_required(VERSION 3.22) 
set(CMAKE_CXX_STANDARD 17)
project(client) 

find_package(Boost COMPONENTS ALL REQUIRED) 

#This line because you are using some depericated Boost APIs 
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS) 

include_directories(${Boost_INCLUDE_DIRS}) 
add_executable(${PROJECT_NAME} projectClient.cpp) 
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES}) 
target_link_libraries(${PROJECT_NAME} pthread ssl crypto) 
# add_executable(client projectClient.cpp)

# # target_link_libraries(client
# #     PRIVATE
# #     Boost::system
# #     Boost::filesystem
# #     Boost::beast
# # )

# target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES}) 
# target_link_libraries(${PROJECT_NAME} mysqlcppconn)