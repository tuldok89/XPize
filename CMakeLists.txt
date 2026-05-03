cmake_minimum_required(VERSION 3.28)

project(XPize2
    VERSION 1.0.0
    DESCRIPTION "A simple comic book reader built with wxWidgets."
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if vcpkg is being used
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)

# Find libarchive
find_package(LibArchive REQUIRED)

# Add executable
add_executable(XPize2
    # Source files
    XPize2.App/MainApp.cpp
    XPize2.App/AppFrame.cpp
    XPize2.App/ArchiveExtractor.cpp
    XPize2.App/DirTraverser.cpp
    XPize2.App/ExtractionDoneEvent.cpp
    XPize2.App/LoadImageEvent.cpp
    XPize2.App/Scroller.cpp
    
    # Header files
    XPize2.App/MainApp.h
    XPize2.App/AppFrame.h
    XPize2.App/ArchiveExtractor.h
    XPize2.App/DirTraverser.h
    XPize2.App/ExtractionDoneEvent.h
    XPize2.App/LoadImageEvent.h
    XPize2.App/Scroller.h
    XPize2.App/Constants.h
    XPize2.App/Util.h
    
    # Resource file (Windows only)
    XPize2.App/XPize2.App.rc
)

# Set Windows subsystem for GUI application
set_target_properties(XPize2 PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# Configure wxWidgets
target_link_libraries(XPize2 PRIVATE wxWidgets::wxWidgets)

# Configure libarchive
target_link_libraries(XPize2 PRIVATE LibArchive::LibArchive)

# Add additional Windows libraries needed by the project
if(WIN32)
    target_link_libraries(XPize2 PRIVATE
        Shlwapi.lib
        WS2_32.lib
        XmlLite.lib
        Crypt32.lib
        Comctl32.lib
        Rpcrt4.lib
        Wininet.lib
        Oleacc.lib
        Uxtheme.lib
    )
endif()

# Add include directories
target_include_directories(XPize2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/XPize2.App
)

# Add preprocessor definitions for Windows
if(WIN32)
    target_compile_definitions(XPize2 PRIVATE
        _WINDOWS
        WXUSINGDLL
    )
    
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(XPize2 PRIVATE
            _DEBUG
        )
    else()
        target_compile_definitions(XPize2 PRIVATE
            NDEBUG
        )
    endif()
endif()

# Set the output directory
set_target_properties(XPize2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy the icon file to the build directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/XPize2.App/XPize-Icon.ico" DESTINATION "${CMAKE_BINARY_DIR}/bin")
