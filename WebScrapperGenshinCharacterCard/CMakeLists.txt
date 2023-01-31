cmake_minimum_required(VERSION 3.21)
set(CMAKE_VERBOSE_MAKEFILE ON)
project(WebScrapperGenshinCharacterCard LANGUAGES CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(VCPKG_TARGET_ANDROID)
<<<<<<< HEAD
=======
    include("cmake/vcpkg_android.cmake")
>>>>>>> 01317acdfac39d17f8ff6501673ad2d733c6678a
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    set(VCPKG_TARGET_ANDROID ON)
    set(CMAKE_ANDROID_STL_TYPE c++_static)
    set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std=c++2a")
    set(CMAKE_ANDROID_ARCH "arm64")
    set(CMAKE_ANDROID_ARCH_ABI "arm64-v8a")
    set(CMAKE_ANDROID_ARCH_TRIPLE "aarch64-linux-android")
    set(CMAKE_ANDROID_NDK_TOOLCHAIN_HOST_TAG "windows-x86_64")
    set(CMAKE_CROSSCOMPILING "TRUE")
    set(CMAKE_CXX_FLAGS "-lstdc++fs -std=c++2a")
else()
    set(CMAKE_CXX_FLAGS "-lstdc++fs -std=c++2a")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../WebScrapperGenshinCharacterCard/)
add_executable(WebScrapperGenshinCharacterCard WebScrapperGenshinCharacterCard.cpp)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL AMD64 OR CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64)
    set_target_properties(WebScrapperGenshinCharacterCard PROPERTIES OUTPUT_NAME "wsgcc_amd64")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL x86 OR CMAKE_SYSTEM_PROCESSOR STREQUAL x86_32)
    set_target_properties(WebScrapperGenshinCharacterCard PROPERTIES OUTPUT_NAME "wsgcc_i386")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL ARM64 OR CMAKE_SYSTEM_PROCESSOR STREQUAL aarch64)
    set_target_properties(WebScrapperGenshinCharacterCard PROPERTIES OUTPUT_NAME "wsgcc_arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES ";armv[6-7]l;" or CMAKE_SYSTEM_PROCESSOR STREQUAL ARM32)
    set_target_properties(WebScrapperGenshinCharacterCard PROPERTIES OUTPUT_NAME "wsgcc_arm32")
elseif(CMAKE_SYSTEM_NAME STREQUAL ANDROID AND CMAKE_SYSTEM_PROCESSOR STREQUAL aarch64)
set_target_properties(WebScrapperGenshinCharacterCard PROPERTIES OUTPUT_NAME "wsgcc_android_arm64")
else()
    set_target_properties(WebScrapperGenshinCharacterCard PROPERTIES OUTPUT_NAME "wsgcc_gen")
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC stdc++fs)

find_package(cpr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

find_package(unofficial-gumbo CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::gumbo::gumbo)

find_package(indicators CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE indicators::indicators)
