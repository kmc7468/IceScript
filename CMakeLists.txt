cmake_minimum_required(VERSION 3.8.0)
project(IceScript)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories("./include")
file(GLOB_RECURSE SOURCE_LIST "./src/*.cpp")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")

set(PYTHON3 CACHE STRING "python3")

add_custom_command(OUTPUT "./src/detail/EastAsianWidthTable.txt"
				   COMMAND ${PYTHON3} ./EastAsianWidthTableGenerator.py
				   WORKING_DIRECTORY "./util")
set_source_files_properties("./src/detail/EastAsianWidthTable.txt" PROPERTIES GENERATED TRUE)
set_source_files_properties("./src/Encoding.cpp" PROPERTIES OBJECT_DEPENDS "./src/detail/EastAsianWidthTable.txt")

add_executable(${PROJECT_NAME} ${SOURCE_LIST} "./src/detail/EastAsianWidthTable.txt")
install(TARGETS ${PROJECT_NAME} DESTINATION "bin")