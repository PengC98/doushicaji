cmake_minimum_required(VERSION 3.5)

project(UAV)
SET(OpenCV_DIR /usr/local/opencv/3.4.3/share/OpenCV)
set(CMAKE_CXX_STANDARD 11)
if(NOT ONBOARDSDK_SOURCE)
    set(ONBOARDSDK_SOURCE "${PROJECT_SOURCE_DIR}/osdk-core")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/libBase/include)
include_directories(${PROJECT_SOURCE_DIR}/libHardWare/serialPort/include)
include_directories(${PROJECT_SOURCE_DIR}/libHardWare/usbCapture/include)
include_directories(${PROJECT_SOURCE_DIR}/libDetection/include)
include_directories(${ONBOARDSDK_SOURCE}/api/inc)
include_directories(${ONBOARDSDK_SOURCE}/utility/inc)
include_directories(${ONBOARDSDK_SOURCE}/hal/inc)
include_directories(${ONBOARDSDK_SOURCE}/protocol/inc)
include_directories(${ONBOARDSDK_SOURCE}/platform/linux/inc)

include_directories(${PROJECT_SOURCE_DIR}/common)
FILE(GLOB SOURCE_FILES *.hpp *.cpp
        ${PROJECT_SOURCE_DIR}/common/dji_linux_environment.cpp
        ${PROJECT_SOURCE_DIR}/common/dji_linux_helpers.cpp
        )
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/libBase)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/libHardWare)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/libDetection)

AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/src DIR_SRCS)

add_executable(example ${DIR_SRCS})

target_link_libraries(example Base)
target_link_libraries(example SerialPort UsbCapture)
target_link_libraries(example pthread)
target_link_libraries(example libDetection)

target_link_libraries(example djiosdk-core)
find_package(OpenCV  REQUIRED)
target_link_libraries(example ${OpenCV_LIBS})
