cmake_minimum_required(VERSION 3.0.0)
project(cppserver VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# if(WIN32)
#     set(OpenCV_DIR "C:/OpenCV-MinGW")
# endif()

# find_package(OpenCV REQUIRED)
# include_directories(${OpenCV_INCLUDE_DIR})

if(WIN32)
    set(WINDOWS_SOURCES
        wtcpserver.h
        wtcpserver.cpp
    )
elseif(APPLE)
    set(MAC_SOURCES
        ltcpserver.h
        ltcpserver.cpp
    )
endif()

add_executable(${PROJECT_NAME} 
    main.cpp
    encode.h
    encode.cpp
    decode.h
    decode.cpp
    ${WINDOWS_SOURCES}
    ${MAC_SOURCES}
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
