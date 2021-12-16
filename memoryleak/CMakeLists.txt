cmake_minimum_required(VERSION 3.1)  #cmake 最低版本要求，低于2.6 构建过程会被终止。 
project(Test2)  #定义工程名称

MESSAGE(STATUS "Project: Test2")               #打印相关消息消息 
MESSAGE(STATUS "Project Directory: ${PROJECT_SOURCE_DIR}")

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -g")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

# SET(CMAKE_BUILE_TYPE DEBUG)                      #指定编译类型
# SET(CMAKE_C_FLAGS_DEBUG "-g -Wall")          #指定编译器 
      
#   ADD_SUBDIRECTORY(utility)                              #添加子目录 没有子目录
#   ADD_SUBDIRECTORY(server)
SET(SOURCE_FILES                                  #设置变量，表示所有的源文件
memorydetect.cc
main.cpp
)
INCLUDE_DIRECTORIES(                          #相关头文件的目录
${PROJET_SOURCE_DIR}/
)

LINK_DIRECTORIES(                                  #相关库文件的目录
    D:/Program files/CodeBlocks/MinGW/x86_64-w64-mingw32/lib
)

ADD_EXECUTABLE(Test2  ${SOURCE_FILES})

SET_TARGET_PROPERTIES(Test2  PROPERTIES    #表示生成的执行文件所在路径
RUNTIME_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/bin")