cmake_minimum_required (VERSION 3.5)
project (PM)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O2 -Wall -DNDEBUG")

include_directories(include /usr/local/include)

add_executable (pm
    main.cpp
    src/nube.cpp
    src/socket_y_sesion.cpp)

target_include_directories (pm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

IF(WIN32)
    target_link_libraries(pm wsock32 ws2_32)
ELSE()
    target_link_libraries(pm pthread)
ENDIF()
