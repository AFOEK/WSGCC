if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT environment variable is not set. Please set it to the vcpkg installation path.")
endif()

set(CMAKE_SYSTEM_NAME Linux)
set(TARGET_ARCH "linux_x86")
set(CMAKE_CXX_FLAGS "-std=c++20")
set(VCPKG_TARGET_TRIPLET x86-linux)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")