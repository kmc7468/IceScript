cmake_minimum_required(VERSION 3.8.0)
project(IceScript)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories("./include")
file(GLOB_RECURSE SOURCE_LIST "./src/*.cpp")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")

add_executable(${PROJECT_NAME} ${SOURCE_LIST})
install(TARGETS ${PROJECT_NAME} DESTINATION "bin")