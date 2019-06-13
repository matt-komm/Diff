project(Diff CXX)
cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

find_package(LLVM REQUIRED)

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
set(CMAKE_CXX_FLAGS ${LLVM_CXXFLAGS} ${CMAKE_CXX_FLAGS})

add_definitions("-fexceptions -std=c++14")
#add_executable(testop testop.cc)
#target_link_libraries(testop ${LLVM_LIBRARIES})


add_executable(llvmtest llvmtest.cc)
target_link_libraries(llvmtest ${LLVM_LIBRARIES} -lz -ldl -lpthread -ltermcap)

