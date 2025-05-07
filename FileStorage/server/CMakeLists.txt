project(FileServer)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(file_server
        src/main.cpp
        src/FileServer.cpp
)

target_link_libraries(file_server ws2_32)
