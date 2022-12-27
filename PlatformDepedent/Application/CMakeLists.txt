# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.19)

# 项目信息
project(Application)

#Cpp 版本
set(CMAKE_CXX_STANDARD 17)

# Windows SDK 版本
set(MIN_SDK_VERSION "10.0.22621.0")

#设置头文件路径
set(spd_log_include ../../3rd_party/Spdlog/include/)
set(boost_include ../../3rd_party/boost_1_80_0/include/)
set(imgui_include ../../3rd_party/imgui/)

#引入头文件
include_directories(
        ${spd_log_include}
        ${boost_include}
        ${imgui_include}
)

# 设置链接库路径
set(boost_link ../../3rd_party/boost_1_80_0/lib/)

# 引入库文件
link_directories(${boost_link})

# 源文件
file(GLOB SOURCES
        AppConfig.h
        DXErr.h
        PCH.h
        Assert.h
        Exceptions.h
        Containers.h
        Window.h
        )

# 源文件
file(GLOB SOURCE_FILES
        DXErr.cpp
        PCH.cpp
        Window.cpp
        )

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_definitions ( -DUNICODE -D_UNICODE )

add_definitions(-D_DEBUG)

# 指定生成lib
add_library(Application ${SOURCES} ${SOURCE_FILES})

target_link_libraries(Application libboost_thread-vc142-mt-x64-1_80 libboost_system-vc142-mt-x64-1_80)