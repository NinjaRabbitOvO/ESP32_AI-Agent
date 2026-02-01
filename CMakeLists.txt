# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
# 上述两行保留官方样板说明，提示必须保持顺序
cmake_minimum_required(VERSION 3.16)  # 指定最低 CMake 版本

include($ENV{IDF_PATH}/tools/cmake/project.cmake)  # 引入 ESP-IDF 提供的项目工具链
# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
# 开启最小化构建，只编译 main 组件及其依赖，缩短构建时间
idf_build_set_property(MINIMAL_BUILD ON)
project(MyAgent_Project)  # 定义项目名称
