project(main)

if(WIN32)
  set(OpenCV_DIR "d:/libs/opencv-3.1.0/build")
else()
  set(OpenCV_DIR "/usr/lib/opencv")
endif()
find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui imgcodecs)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(resizeIms main.cpp)
target_link_libraries(resizeIms ${OpenCV_LIBS})
