cmake_minimum_required(VERSION 4.0.0)
project(diary LANGUAGES CXX C)

include(GNUInstallDirs)
include(ExternalProject)
include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# --- Main Executable --------------------------------------------------------- 
add_executable(${CMAKE_PROJECT_NAME}
    source/main.cpp
    source/menu.cpp
    source/utils.cpp
    source/editor.cpp
    source/db.cpp
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
install(TARGETS ${CMAKE_PROJECT_NAME})

# --- Fetch FTXUI ------------------------------------------------------------- 
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/arthursonzogni/ftxui.git
  GIT_TAG        v5.0.0
  GIT_PROGRESS   TRUE
  GIT_SHALLOW    TRUE
  EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(ftxui)
# --- Build SQLite ------------------------------------------------------------ 
set(SQLITE_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/sqlite-build")
set(SQLITE_C_FILE "${SQLITE_BUILD_DIR}/sqlite3.c")
set(SQLITE_H_FILE "${SQLITE_BUILD_DIR}/sqlite3.h")
ExternalProject_Add(
    sqlite_ep
    GIT_REPOSITORY https://github.com/sqlite/sqlite.git
    GIT_TAG master
    BINARY_DIR ${SQLITE_BUILD_DIR}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} sqlite3.c
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
    BUILD_BYPRODUCTS ${SQLITE_C_FILE} ${SQLITE_H_FILE}
)
add_library(sqlite3_static STATIC
    ${SQLITE_C_FILE}
    ${SQLITE_H_FILE}
)
add_dependencies(sqlite3_static sqlite_ep)
target_include_directories(sqlite3_static PUBLIC
    ${SQLITE_BUILD_DIR}
)
target_compile_definitions(sqlite3_static PRIVATE
    SQLITE_THREADSAFE=1
)
# ------------------------------------------------------------------------------

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE sqlite3_static
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component  
)

install(FILES assets/diary.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

install(FILES assets/diary.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
)


