cmake_minimum_required(VERSION 2.8.12)
project(Alpsdemo)

find_package(ALPSCore REQUIRED)
find_package(GFTools REQUIRED)

add_executable(alpsdemo main.cpp process.cpp)
target_link_libraries(alpsdemo ${ALPSCore_LIBRARIES} ${GFTools_LIBRARIES})
target_include_directories(alpsdemo PRIVATE ${GFTools_INCLUDE_DIRS})
