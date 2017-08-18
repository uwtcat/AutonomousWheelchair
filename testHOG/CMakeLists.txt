project(main)

set(CMAKE_CXX_STANDARD 11)


       SET(VERSION_REQ_OCV "2.4.13")
       SET(VERSION_REQ_CUDA "")

       unset(CUDA_USE_STATIC_CUDA_RUNTIME CACHE)
       option(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
       message ("CUDA_USE_STATIC_CUDA_RUNTIME : ${CUDA_USE_STATIC_CUDA_RUNTIME}")
       ##to prevent from opencv_dep_cudart dependencies error...
       ## cmake with -DCUDA_USE_STATIC_CUDA_RUNTIME=false can also be called.


set(OPENCV_DIR "/usr/lib/opencv")
find_package(OpenCV REQUIRED COMPONENTS core highgui ml objdetect)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(testHOG main.cpp)
target_link_libraries(testHOG ${OpenCV_LIBS})
