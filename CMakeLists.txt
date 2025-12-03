cmake_minimum_required(VERSION 4.0)
project(Flux)

# 输出目录
set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin CACHE PATH "Output directory for binaries")

# 确保输出目录存在
file(MAKE_DIRECTORY ${BIN_DIR})


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BIN_DIR})

add_subdirectory(Core/external/glm)
add_subdirectory(Core/external/GLFW)

add_subdirectory(Core)

add_subdirectory(Application)