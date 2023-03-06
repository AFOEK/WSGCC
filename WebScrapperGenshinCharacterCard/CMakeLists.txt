cmake_minimum_required(VERSION 3.23)
set(CMAKE_VERBOSE_MAKEFILE ON)
project(WebScrapperGenshinCharacterCard LANGUAGES CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CROSSCOMPILING "TRUE")

if(VCPKG_TARGET_ANDROID)
    set(CMAKE_THREAD_LIBS_INIT "-pthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    set(VCPKG_TARGET_ANDROID ON)
    set(CMAKE_ANDROID_STL_TYPE c++_static)
    set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std=c++2a")
else()
    set(CMAKE_CXX_FLAGS "-lstdc++fs -std=c++2a")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../WebScrapperGenshinCharacterCard/)
add_executable(WebScrapperGenshinCharacterCard WebScrapperGenshinCharacterCard.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "wsgcc")

if(CMAKE_SYSTEM_NAME STREQUAL Windows OR CMAKE_SYSTEM_NAME STREQUAL Linux OR CMAKE_SYSTEM_NAME STREQUAL Darwin)
    target_link_libraries(${PROJECT_NAME} PUBLIC stdc++fs)
endif()

find_package(cpr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

find_package(unofficial-gumbo CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::gumbo::gumbo)

find_package(indicators CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE indicators::indicators)

find_package(argparse CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE argparse::argparse)