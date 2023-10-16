add_executable(server projectServer.cpp)

target_link_libraries(server
    PRIVATE
    Boost::system
    Boost::filesystem
    Boost::beast
    mysqlcppconn)
