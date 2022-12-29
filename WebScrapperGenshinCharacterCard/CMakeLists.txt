cmake_minimum_required(VERSION 3.21)
set(CMAKE_VERBOSE_MAKEFILE ON)
project(WebScrapperGenshinCharacterCard LANGUAGES CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-lstdc++fs -std=c++2a")
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