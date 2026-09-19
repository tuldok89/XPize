# Building XPize2

XPize2 targets wxWidgets 2.8.12 compiled with MinGW 3.14 / gcc 3.4.5
(C++98). It extracts comic book archives with 7-Zip instead of libarchive, so
no vcpkg is required.

## Prerequisites

- CMake 3.16 or higher (tested with CMake 4.4.3)
- MinGW 3.14 (gcc 3.4.5, C++98) on the PATH
- A wxWidgets 2.8.12 build for MinGW (see below)
- 7-Zip command line tool (`7z.exe`) on the PATH at runtime

### wxWidgets 2.8.12 setup

Build wxWidgets as a shared (DLL), non-monolithic, Unicode + MSLU library for
gcc. The expected layout below `<base>` (e.g. `C:\Code\wxWidgets-2.8.12`):

```
<base>/include                generic headers
<base>/lib/gcc_dll/mswu       wx/setup.h (per-build configuration)
<base>/lib/gcc_dll            import libraries and .dll files
```

Compile the compiler flags that define the Unicode build into the
application:

```
-D_UNICODE -DwxUSE_UNICODE_MSLU=1 -DWXUSINGDLL -DHAVE_W32API_H -D__WXMSW__
```

These are applied automatically by the CMakeLists.txt when `WIN32` is set.

## Building

```powershell
cmake -G "MinGW Makefiles" `
  -DWXWIDGETS_DIR="C:\Code\wxWidgets-2.8.12" `
  -DCMAKE_CXX_COMPILER="C:\MinGW\bin\g++.exe" `
  -DXPIZE2_7ZIP_CLI="7z" `
  -S .
  -B build

cmake --build build
```

The executable and the required wxWidgets runtime DLLs are copied into
`build/bin/`.

## CMake Options

- `WXWIDGETS_DIR` - Base directory of the wxWidgets 2.8 installation. When
  empty, CMake probes the compiler's default include/library search paths.
- `XPIZE2_7ZIP_CLI` - 7-Zip command line executable used for extraction
  (default: `7z`). It is baked into the binary as a string literal, so it
  must be reachable from the machine running XPize2.

## Runtime requirements

- The wxWidgets runtime DLLs next to the executable (`wxbase28u_gcc_custom.dll`,
  `wxmsw28u_core_gcc_custom.dll`, `wxmsw28u_adv_gcc_custom.dll`, and their
  dependencies) - already copied by the build.
- `gdiplus.dll` (Windows 98/Me do not ship it; provide it alongside the
  executable).
- `unicows.dll` on Windows 9x (the Unicode layer); not needed on Windows NT.
- A 7-Zip CLI binary at the path given by `XPIZE2_7ZIP_CLI` (e.g. `7z.exe`),
  or provide `7z.exe` alongside the executable.

The binary targets the Win32 API and is intended to run on Windows 98 SE and
later; the XP Common Controls 6.0 manifest is deliberately not embedded.