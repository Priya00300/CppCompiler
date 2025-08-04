cmake_minimum_required(VERSION 3.15)
project(MyCompiler LANGUAGES CXX)

# Find Flex/Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# Generate lexer/parser
flex_target(LEXER src/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
bison_target(PARSER src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)

# Add executable
add_executable(my_compiler 
    src/main.cpp
    ${BISON_PARSER_OUTPUTS} 
    ${FLEX_LEXER_OUTPUTS}
)

# C++20 support
target_compile_features(my_compiler PRIVATE cxx_std_20)