# Building XPize2 with CMake

## Prerequisites

- CMake 3.28 or higher
- A C++20 compatible compiler (MSVC 2022 or later recommended)
- vcpkg package manager

## Building

### Windows (with vcpkg)

1. Ensure vcpkg is installed and configured:

```powershell
# Set the VCPKG_ROOT environment variable if not already set
$env:VCPKG_ROOT = "C:\path\to\vcpkg"
```

2. Install dependencies (if not already installed):

```powershell
vcpkg install wxwidgets libarchive --triplet=x64-windows
vcpkg install wxwidgets libarchive --triplet=arm64-windows
```

3. Configure and build:

```powershell
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -G "Visual Studio 17 2022" -A x64

# Build
cmake --build . --config Release
```

The executable will be located in `build/bin/`.

## CMake Options

- `-G "Visual Studio 17 2022"` - Generator (Visual Studio 2022)
- `-A x64` or `-A Win32` or `-A ARM64` - Architecture
- `-DCMAKE_BUILD_TYPE=Release` or `-DCMAKE_BUILD_TYPE=Debug` - Build type

## Using vcpkg Manifest

This project uses vcpkg's manifest mode (vcpkg.json). The dependencies are:
- wxwidgets (3.2.8.1)
- libarchive with features: bzip2, lzma, zstd (3.8.5)
