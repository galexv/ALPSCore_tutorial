cmake_minimum_required(VERSION 2.8.12)
project(Alpsdemo)

find_package(ALPSCore REQUIRED)

add_executable(alpsdemo main.cpp simulation.cpp)
target_link_libraries(alpsdemo ${ALPSCore_LIBRARIES})
