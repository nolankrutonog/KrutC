cmake_minimum_required(VERSION 3.29.3)

project(krutc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find LLVM package
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM directories
include_directories(${LLVM_INCLUDE_DIRS})
# include_directories(${CMAKE_SOURCE_DIR}/../../llvm-project/llvm/lib)
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Include project source directories
include_directories(${CMAKE_SOURCE_DIR}/src/frontend
                    ${CMAKE_SOURCE_DIR}/src/frontend/include)

# Define source files
set(SOURCES src/main.cpp 
            src/frontend/lexer.cpp 
            src/frontend/parser.cpp 
            src/frontend/tree.cpp 
            src/frontend/typechecker.cpp 
            src/frontend/scopetable.cpp
            src/backend/codegen.cpp)

# Add executable target
add_executable(krutc ${SOURCES})

# Link LLVM libraries to the target
llvm_map_components_to_libnames(llvm_libs
                                support
                                core
                                irreader
                                executionengine
                                mcjit
                                native
                                orcjit
)
target_link_libraries(krutc ${llvm_libs})

# Ensure the LLVM libraries are found
target_include_directories(krutc PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(krutc PRIVATE ${LLVM_DEFINITIONS})