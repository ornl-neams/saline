# toolchains/macos-clang-arm64.cmake

# Host / target system
set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR arm64)

# Compilers: use Apple Clang from Xcode / CLT
# Let CMake find them in PATH; override if you really want explicit paths.
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_Fortran_COMPILER gfortran)

# Architectures: build for native arm64
set(CMAKE_OSX_ARCHITECTURES arm64)

# Use the current macOS SDK
execute_process(
    COMMAND xcrun --sdk macosx --show-sdk-path
    OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Optional: pin a specific version:
# set(CMAKE_C_COMPILER clang-18)
# set(CMAKE_CXX_COMPILER clang++-18)

# Optionally: if you *really* want to pin the minimum OS version, you can
# do it here, but consider whether that's project policy instead of env.
# set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0")
