set(pathToProject /home/tmd/Desktop/workspace/gr)
# https://medium.com/@kashafahmed/a-proper-beginners-guide-to-installing-opencv-android-in-android-studio-updated-5fe7f3399e1c
set(pathToOpenCv /home/tmd/Downloads/OpenCV-android-sdk)
cmake_minimum_required(VERSION 3.4.1) # opencv-3.4.3-android-sdk

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

include_directories(${pathToOpenCv}/sdk/native/jni/include)

file(GLOB srcs *.cpp *.c)
file(GLOB hdrs *.hpp *.h)
add_library(native-lib SHARED ${srcs} ${hdrs})

add_library( lib_opencv SHARED IMPORTED )

set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION ${pathToProject}/app/src/main/jniLibs/${ANDROID_ABI}/libopencv_java3.so)

find_library( log-lib log )

target_link_libraries( native-lib ${log-lib} lib_opencv)