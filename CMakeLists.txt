cmake_minimum_required (VERSION 3.5)
project (PM)

find_package(Boost 1.58 REQUIRED)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O2 -Wall -DNDEBUG")

#include_directories(include /usr/local/include/google/tensorflow)
include_directories(include /usr/local/include)

IF (WIN32)
  include_directories(C:/include)
ELSE()
  # set stuff for other systems
ENDIF()

# Add executable called "stormbreaker" that is built from the source files
# "main.cpp" and "*.cxx". The extensions are automatically found.
add_executable (pm
    main.cpp
    configuracion.cpp
    src/autenticacion.cpp
    src/estructuras.cpp
    src/nube.cpp
    src/socket_y_sesion.cpp)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(pm ${Boost_LIBRARIES})
endif()

target_include_directories (pm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries (pm ${OpenCV_LIBS})
IF(WIN32)
    target_link_libraries(pm wsock32 ws2_32)
ELSE()

ENDIF()