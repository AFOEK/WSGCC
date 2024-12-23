cmake_minimum_required(VERSION 3.23)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

if(NOT WIN32)
    string(ASCII 27 Esc)
    set(ColourReset "${Esc}[m")
    set(ColourBold  "${Esc}[1m")
    set(Red         "${Esc}[31m")
    set(Green       "${Esc}[32m")
endif()

project(WebScrapperGenshinCharacterCard LANGUAGES CXX)

if(NOT DEFINED TARGET_ARCH)
    message(FATAL_ERROR "${Red}TARGET_ARCH is not defined${ColourReset}")
else()
    message(STATUS "${Green}TARGET_ARCH = ${TARGET_ARCH}${ColourReset}")
    message(STATUS "${Green}VCPKG TARGET = ${VCPKG_TARGET_TRIPLET}${ColourReset}")
    message(STATUS "${Green}VCPKG_ROOT = $ENV{VCPKG_ROOT}${ColourReset}")
    message(STATUS "${Green}CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}${ColourReset}")
endif()


set(OUTPUT_NAME "wsgcc_${TARGET_ARCH}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../WebScrapperGenshinCharacterCard/)

add_executable(${OUTPUT_NAME} WebScrapperGenshinCharacterCard.cpp)

find_package(cpr REQUIRED)
target_link_libraries(${OUTPUT_NAME} PRIVATE cpr::cpr)

find_package(CURL REQUIRED)
target_link_libraries(${OUTPUT_NAME} PRIVATE CURL::libcurl)

find_package(unofficial-gumbo REQUIRED)
target_link_libraries(${OUTPUT_NAME} PRIVATE unofficial::gumbo::gumbo)

find_package(indicators REQUIRED)
target_link_libraries(${OUTPUT_NAME} PRIVATE indicators::indicators)

find_package(argparse REQUIRED)
target_link_libraries(${OUTPUT_NAME} PRIVATE argparse::argparse)

find_path(TERMCOLOR_INCLUDE_DIRS "termcolor/termcolor.hpp")
target_include_directories(${OUTPUT_NAME} PRIVATE ${TERMCOLOR_INCLUDE_DIRS})