cmake_minimum_required(VERSION 3.16)

project(XPize2
    VERSION 1.0.0
    DESCRIPTION "A simple comic book reader built with wxWidgets."
    LANGUAGES CXX
)

# Path to the base directory of the wxWidgets 2.8 installation.
# If left empty, CMake probes the compiler default search paths.
set(WXWIDGETS_DIR "" CACHE PATH "Base directory of the wxWidgets 2.8 installation")

# 7-Zip command line executable used to extract comic book archives.
# Passed to the compiler as a string literal via a compile definition.
set(XPIZE2_7ZIP_CLI "7z" CACHE STRING "7-Zip command line executable used for extraction")

# ---------------------------------------------------------------------------
# wxWidgets discovery (wxWidgets 2.8 pre-dates CMake support, so the layout is
# configured manually). The supported layout for the gcc DLL build is:
#   <base>/include                generic headers
#   <base>/lib/gcc_dll/mswu       wx/setup.h (per-build configuration)
#   <base>/lib/gcc_dll            import libraries and .dll files
# ---------------------------------------------------------------------------
set(WX_INCLUDE_DIR "")
set(WX_SETUP_DIR "")
set(WX_LIB_DIR "")

if(WXWIDGETS_DIR)
    set(WX_INCLUDE_DIR "${WXWIDGETS_DIR}/include")
    set(WX_SETUP_DIR "${WXWIDGETS_DIR}/lib/gcc_dll/mswu")
    set(WX_LIB_DIR "${WXWIDGETS_DIR}/lib/gcc_dll")
else()
    find_path(WX_INCLUDE_DIR wx/wx.h)
    find_path(WX_SETUP_DIR wx/setup.h)
    find_library(WX_WXM28U_CORE_LIB wxmsw28u_core)
    if(WX_WXM28U_CORE_LIB)
        get_filename_component(WX_LIB_DIR "${WX_WXM28U_CORE_LIB}" PATH)
    endif()
endif()

if(NOT WX_INCLUDE_DIR OR NOT WX_SETUP_DIR OR NOT WX_LIB_DIR)
    message(FATAL_ERROR
        "wxWidgets 2.8 not found. Set -DWXWIDGETS_DIR=<base directory of the "
        "wxWidgets 2.8 installation>\nor make sure wxWidgets headers and "
        "libraries are reachable from the compiler's default search paths.\n"
        "Expected layout: <base>/include, <base>/lib/gcc_dll/mswu, <base>/lib/gcc_dll")
endif()

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------
link_directories(${WX_LIB_DIR})

add_executable(XPize2
    # Source files
    XPize2.App/MainApp.cpp
    XPize2.App/AppFrame.cpp
    XPize2.App/ArchiveExtractor.cpp
    XPize2.App/DirTraverser.cpp
    XPize2.App/ExtractionDoneEvent.cpp
    XPize2.App/LoadImageEvent.cpp
    XPize2.App/NavigationEvent.cpp
    XPize2.App/Scroller.cpp
    XPize2.App/StatusBar.cpp

    # Header files
    XPize2.App/MainApp.h
    XPize2.App/AppFrame.h
    XPize2.App/ArchiveExtractor.h
    XPize2.App/DirTraverser.h
    XPize2.App/ExtractionDoneEvent.h
    XPize2.App/LoadImageEvent.h
    XPize2.App/NavigationEvent.h
    XPize2.App/Scroller.h
    XPize2.App/StatusBar.h
    XPize2.App/Constants.h
    XPize2.App/Util.h

    # Resource file (Windows only)
    XPize2.App/XPize2.App.rc
)

# Windows subsystem GUI application
set_target_properties(XPize2 PROPERTIES
    WIN32_EXECUTABLE TRUE
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(XPize2 PRIVATE
    ${WX_SETUP_DIR}
    ${WX_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/XPize2.App
)

# 7-Zip CLI used at runtime (compile-time string literal)
target_compile_definitions(XPize2 PRIVATE
    XPIZE2_7ZIP_CLI="${XPIZE2_7ZIP_CLI}"
)

if(WIN32)
    # Matches the Unicode + MSLU shared (DLL) build of wxWidgets 2.8.12
    target_compile_definitions(XPize2 PRIVATE
        _WINDOWS
        WXUSINGDLL
        _UNICODE
        wxUSE_UNICODE_MSLU=1
        HAVE_W32API_H
        __WXMSW__
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(XPize2 PRIVATE
            _DEBUG
            __WXDEBUG__
        )
    else()
        target_compile_definitions(XPize2 PRIVATE
            NDEBUG
        )
    endif()

    # Let wx/msw/wx.rc embed its own RT_MANIFEST (id 1/24, resource
    # "wx/msw/wx.manifest", the standard Common Controls 6.0 / XP manifest).
    # On Windows 98 the side-by-side loader has no SxS infrastructure, so it
    # ignores the embedded manifest and falls back to the OS comctl32 — the
    # same behaviour as 7-Zip's own binary. Do NOT set wxUSE_NO_MANIFEST here.
endif()

target_link_libraries(XPize2 PRIVATE
    wxmsw28u_core
    wxmsw28u_adv
    wxbase28u
    wxtiff
    wxjpeg
    wxpng
    wxzlib
    wxregexu
    wxexpat
    unicows
    gdiplus
)

if(WIN32)
    target_link_libraries(XPize2 PRIVATE
        kernel32
        user32
        gdi32
        comdlg32
        winspool
        winmm
        shell32
        comctl32
        ole32
        oleaut32
        uuid
        rpcrt4
        advapi32
        wsock32
        odbc32
        shlwapi
    )
endif()

# Copy the icon and runtime DLLs to the output directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/XPize2.App/XPize-Icon.ico" DESTINATION "${CMAKE_BINARY_DIR}/bin")

if(WIN32 AND WX_LIB_DIR)
    file(GLOB WX_RUNTIME_DLLS "${WX_LIB_DIR}/*_gcc_custom.dll")
    file(COPY ${WX_RUNTIME_DLLS} DESTINATION "${CMAKE_BINARY_DIR}/bin")
endif()