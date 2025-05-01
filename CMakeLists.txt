cmake_minimum_required(VERSION 4.0.0)
project(diary LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(SQLITE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/sqlite")
add_library(sqlite3_static STATIC
    "${SQLITE_SOURCE_DIR}/sqlite3.c"
)
target_include_directories(sqlite3_static PUBLIC
    "${SQLITE_SOURCE_DIR}"
)
target_compile_definitions(sqlite3_static PRIVATE
    SQLITE_THREADSAFE=1
)


include_directories(include/)
add_executable(${CMAKE_PROJECT_NAME}
    source/main.cpp
)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE sqlite3_static)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
install(TARGETS ${CMAKE_PROJECT_NAME})

