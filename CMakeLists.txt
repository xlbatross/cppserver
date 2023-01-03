cmake_minimum_required(VERSION 3.0.0)
project(cppserver VERSION 0.1.0)

include(CTest)
enable_testing()

if(WIN32)
    set(OpenCV_DIR "C:/OpenCV-MinGW")
endif()

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIR})

add_executable(${PROJECT_NAME} 
    main.cpp
    tcpserver.h
    tcpserver.cpp
    encode.h
    encode.cpp
    decode.h
    decode.cpp
)
target_link_libraries(${PROJECT_NAME} 
    ${LIBOPENCV}
    ${OpenCV_LIBRARIES}
)
if(WIN32)
  target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
