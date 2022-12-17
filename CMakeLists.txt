# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.19)                    #Cmake版本 3.19

# 项目信息
project(DistEngine)                                     #主工程 DistEngine

# Cpp 版本
set(CMAKE_CXX_STANDARD 17)                              #C++17版本

# 指定生成子项目
add_subdirectory(PlatformDepedent/Core Core)            #编译子工程 Core

# 设置第三方库头文件路径
set(spd_log_include 3rd_party/Spdlog/include/)          #SpdLog
set(boost_include 3rd_party/boost_1_80_0/include/)      #Boost
set(imgui_include 3rd_party/imgui/)                     #Imgui

# 设置子工程头文件路径
set(core_include PlatformDepedent/Core/)                #Core 头文件


# 引入头文件
include_directories(
        ${spd_log_include}
        ${boost_include}
        ${imgui_include}
        ${core_include}
)

# 设置第三方库链接库路径
set(boost_link 3rd_party/boost_1_80_0/lib/)             #Boost lib path

# 设置子工程链接库路径
set(core_link cmake-build-debug/Core)                   #Boost lib

# 引入库文件
link_directories(  
        ${boost_link}
        ${core_link}
)

# 指定编译子目录
add_executable(DistEngine EngineApp/main.cpp)

# 链接外部库
target_link_libraries(DistEngine libboost_thread-vc142-mt-x64-1_80 libboost_system-vc142-mt-x64-1_80 Core)