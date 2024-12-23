if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT environment variable is not set. Please set it to the vcpkg installation path.")
endif()

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file" FORCE)
set(VCPKG_TARGET_TRIPLET "x86-linux" CACHE STRING "vcpkg target triplet")
set(TARGET_ARCH "linux_x86" CACHE STRING "Target Architecture")
set(CMAKE_SYSTEM_PROCESSOR x86 CACHE STRING "Target Binary")
set(CMAKE_CXX_FLAGS "-m32 -std=c++20")
set(CMAKE_C_FLAGS "-m32")
set(CMAKE_EXE_LINKER_FLAGS "-m32")
set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_PREFIX_PATH "$ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}")