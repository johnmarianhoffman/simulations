cmake_minimum_required (VERSION 3.1 FATAL_ERROR)
project (LV_KMC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-pthread -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

#file( GLOB SOURCES *.cpp )
add_executable(LV_KMC main.cpp)
add_executable(test_boundary_conditions test_boundary_conditions.cpp)

if ((UNIX) AND (NOT APPLE))
message("LINUX DETECTED")
find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)
target_link_libraries(LV_KMC -lX11 -lstdc++fs ${OPENGL_LIBRARIES} ${ZLIB_LIBRARIES} ${PNG_LIBRARIES})
endif()