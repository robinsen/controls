cmake_minimum_required(VERSION 3.1)  #cmake 最低版本要求，低于2.6 构建过程会被终止。 
project(ClientSocket)  #定义工程名称

MESSAGE(STATUS "Project: ClientSocket")               #打印相关消息消息 
MESSAGE(STATUS "Project Directory: ${PROJECT_SOURCE_DIR}")

if (WIN32)
	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /MP /wd4200")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /wd4200 /std:c++11")
else ()
	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -fPIC -pipe -std=gnu90")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -pipe -std=c++11 -fno-exceptions -Wno-invalid-offsetof")
endif ()

# SET(CMAKE_BUILE_TYPE DEBUG)                      #指定编译类型
# SET(CMAKE_C_FLAGS_DEBUG "-g -Wall")          #指定编译器 
      
#   ADD_SUBDIRECTORY(...)                              #添加子目录 没有子目录
#   ADD_SUBDIRECTORY(...)
SET(SOURCE_FILES                                  #设置变量，表示所有的源文件
clientsocket.cpp
helloserver.cpp
processuser.cpp
http_conn.cpp
webserver.cpp
log.cpp
testutil.cpp
main.cpp
)

INCLUDE_DIRECTORIES(                          #相关头文件的目录
${PROJET_SOURCE_DIR}/
)

ADD_EXECUTABLE(ClientSocket  ${SOURCE_FILES})

if(WIN32)
    TARGET_LINK_LIBRARIES(ClientSocket wsock32)
endif(WIN32)

TARGET_LINK_LIBRARIES(ClientSocket pthread)

SET_TARGET_PROPERTIES(ClientSocket  PROPERTIES    #表示生成的执行文件所在路径
RUNTIME_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/bin")

ADD_DEFINITIONS(-Wall -fexceptions)