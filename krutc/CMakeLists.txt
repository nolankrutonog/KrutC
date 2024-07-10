# cmake_minimum_required(VERSION 3.29.3)

# # Set the project name
# project(krutc)

# # Specify the C++ standard
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# # Use llvm-config to find LLVM
# execute_process(
#     COMMAND /opt/homebrew/opt/llvm/bin/llvm-config --prefix
#     OUTPUT_VARIABLE LLVM_INSTALL_DIR
#     OUTPUT_STRIP_TRAILING_WHITESPACE
# )

# find_package(LLVM REQUIRED CONFIG PATHS ${LLVM_INSTALL_DIR}/lib/cmake/llvm NO_DEFAULT_PATH)


# # Include the LLVM headers
# include_directories(${LLVM_INCLUDE_DIRS})
# add_definitions(${LLVM_DEFINITIONS})

# # Source files
# set(SOURCES frontend/main.cpp frontend/lexer.cpp frontend/parser.cpp 
#             frontend/tree.cpp frontend/typechecker.cpp frontend/scopetable.cpp)

# # Add the executable
# add_executable(krutc ${SOURCES})

# # Link LLVM libraries
# llvm_map_components_to_libnames(LLVM_LIBS support core irreader)
# target_link_libraries(krutc ${LLVM_LIBS})

cmake_minimum_required(VERSION 3.29.3)

project(krutc)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

set(SOURCES frontend/main.cpp frontend/token.cpp frontend/lexer.cpp frontend/parser.cpp 
            frontend/tree.cpp frontend/typechecker.cpp frontend/scopetable.cpp)

add_executable(krutc ${SOURCES})

llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(krutc ${llvm_libs})
