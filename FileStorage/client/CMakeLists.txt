cmake_minimum_required(VERSION 3.12)
project(FileClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets REQUIRED COMPONENTS core base net)
include(${wxWidgets_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(file_client
        src/MyApp.cpp
        src/MainFrame.cpp
        src/FIleClient.cpp
)

target_link_libraries(file_client
        ${wxWidgets_LIBRARIES}
        ws2_32   # для getaddrinfo/freeaddrinfo
)
