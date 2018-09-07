cmake_minimum_required(VERSION 3.12)
project(P1_Hash_Analysis)

set(CMAKE_CXX_STANDARD 14)

set(LIB_DIR "/Users/ajwood/Dropbox/FSU/COP 4531/LIB")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${LIB_DIR}/cpp)
include_directories(${LIB_DIR}/tcpp)
include_directories(${LIB_DIR}/graph)
include_directories(${LIB_DIR}/tests)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(HashAnalysis fhtbl.cpp)
add_executable(HashEval hasheval.cpp)
