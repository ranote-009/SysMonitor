add_executable(client projectClient.cpp)

target_link_libraries(client
    PRIVATE
    Boost::system
    Boost::filesystem
    Boost::beast
)
