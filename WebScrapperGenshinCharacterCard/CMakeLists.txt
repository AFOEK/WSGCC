cmake_minimum_required(VERSION 3.23)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

project(WebScrapperGenshinCharacterCard LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../WebScrapperGenshinCharacterCard/)
add_executable(WebScrapperGenshinCharacterCard WebScrapperGenshinCharacterCard.cpp)

find_package(cpr REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC cpr::cpr)

find_package(CURL REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC CURL::libcurl)

find_package(unofficial-gumbo REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC unofficial::gumbo::gumbo)

find_package(indicators REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC indicators::indicators)

find_package(argparse REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC argparse::argparse)

find_path(TERMCOLOR_INCLUDE_DIRS "termcolor/termcolor.hpp")
target_include_directories(${PROJECT_NAME} PUBLIC ${TERMCOLOR_INCLUDE_DIRS})
