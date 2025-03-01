if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT environment variable is not set. Please set it to the vcpkg installation path.")
endif()

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file" FORCE)
set(VCPKG_TARGET_TRIPLET "arm64-linux" CACHE STRING "vcpkg target triplet")
set(CMAKE_SYSTEM_PROCESSOR "aarch64" CACHE STRING "Target Binary")
set(CMAKE_SYSTEM_NAME Linux)
set(TARGET_ARCH "linux_arm64" CACHE STRING "Target Architecture")
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(CMAKE_C_FLAGS "-march=armv8-a")
set(CMAKE_CXX_FLAGS "-march=armv8-a")
set(CMAKE_LINKER_FLAGS "-march=armv8-a")
set(CMAKE_CXX_FLAGS "-std=c++20")

set(CMAKE_PREFIX_PATH "$ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}")