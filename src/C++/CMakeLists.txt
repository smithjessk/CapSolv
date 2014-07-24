cmake_minimum_required(VERSION 2.8)
project( DisplayImage )

set(CMAKE_MODULE_PATH './cmake-modules')

list( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs") 
## Use c++11

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( DisplayImage DisplayImage.cpp )
add_executable( trainSVM trainSVM.cpp )
add_executable( process process.cpp )
add_executable( test test.cpp )

target_link_libraries( DisplayImage ${OpenCV_LIBS} )
target_link_libraries( trainSVM ${OpenCV_LIBS} )
target_link_libraries( process ${OpenCV_LIBS} )
target_link_libraries( test ${OpenCV_LIBS} )