#remember to install opencv and opencv-devel
cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 14)
project( BarcodeTest )
find_package( OpenCV REQUIRED )
add_executable( BarcodeTest BarcodeTest.cpp )
target_link_libraries( BarcodeTest ${OpenCV_LIBS} )
