cmake_minimum_required(VERSION 3.12.0)
project(escalonador VERSION 0.1.0)

file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.cc")
add_executable(${PROJECT_NAME} ${SOURCES})

# target_include_directories(${PROJECT_NAME} PRIVATE "include")