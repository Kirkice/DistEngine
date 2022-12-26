# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.19)

# 项目信息
project(Core)

#Cpp 版本
set(CMAKE_CXX_STANDARD 17)

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
        LogSystem/Log.h
        Mathf/Color.h
        Mathf/Mathf.h
        Mathf/MathHelper.h
        Mathf/Matrix.h
        Mathf/Quaternion.h
        Mathf/Random.h
        Mathf/Vector2.h
        Mathf/Vector3.h
        Mathf/Vector4.h
        )

# 源文件
file(GLOB SOURCE_FILES
        LogSystem/Log.cpp
        Mathf/Color.cpp
        Mathf/MathHelper.cpp
        Mathf/Matrix.cpp
        Mathf/Quaternion.cpp
        Mathf/Random.cpp
        Mathf/Vector2.cpp
        Mathf/Vector3.cpp
        Mathf/Vector4.cpp
        )

# 指定生成lib
add_library(Core ${SOURCES} ${SOURCE_FILES})

target_link_libraries(Core libboost_thread-vc142-mt-x64-1_80 libboost_system-vc142-mt-x64-1_80)