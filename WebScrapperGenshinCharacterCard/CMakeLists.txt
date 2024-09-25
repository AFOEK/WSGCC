cmake_minimum_required(VERSION 3.23)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(CMAKE_CROSSCOMPILING "TRUE")
OPTION(BUILD_32 "Build for 32 bit" OFF)

if(VCPKG_TARGET_ANDROID)
    message(STATUS "Set up Android build variable")
    set(CMAKE_THREAD_LIBS_INIT "-pthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    set(VCPKG_TARGET_ANDROID ON)
    set(CMAKE_ANDROID_STL_TYPE "c++_static")
    set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std=c++20")
else()
    set(CMAKE_CXX_FLAGS "-std=c++20")
endif()

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
