# CMake build system for Dear ImGui
# =================================
#
# Build instructions:
# 1. Install latest CMake
#   * Windows: https://cmake.org/download/ (Tick checkbox to place cmake in system PATH)
#   * Linux: from your favorite package manager
#   * MacOS: brew install cmake
# 2. Open command prompt in directory containing "imgui" and "imgui_dev" folders
#   * Windows: open Visual Studio tools command prompt or specify -G "Generator Name" (quotes are important).
#   * Other platforms: Specifying generator name is optional.
# 3. Create a build directory and configure build
#    git clone https://github.com/ocornut/imgui.git
#    mkdir cmake-build
#    cd cmake-build
#    cmake <optional build parameters> ../imgui
# 4. Build
#   * Windows (Visual Studio generators): Open generated imgui_dev.sln
#   * All platforms: cmake --build .
#
# Example of using Dear ImGui in your CMake project:
#
#   # Set various library options:
#   set(IMGUI_SDL_TARGET sdl CACHE STRING "" FORCE)
#   # Include Dear ImGui into build.
#   add_subdirectory(path/to/imgui)
#   <..>
#   # Link Dear ImGui to your application:
#   #  imgui - a main library target
#   #  imgui-sdl2 - a platform backend target
#   #  imgui-dx11 - a graphics backend target
#   target_link_libraries(YourApplication PUBLIC imgui imgui-sdl2 imgui-dx11)
#
# A list of defined targets:
#   imgui-sdl2: platform backend for Windows/Linux/MacOS/etc using SDL2.
#   imgui-glfw: platform backend for Windows/Linux/MacOS/etc using GLFW.
#   imgui-osx: platform backend for MacOS.
#   imgui-win32: platform backend for Windows.
#   imgui-glut: platform backend for Windows/Linux/MacOS using glut/freeglut with OpenGL.
#   imgui-metal: graphics backend for MacOS using Metal graphics API.
#   imgui-opengl3: graphics backend for Windows/Linux/MacOS/etc using OpenGL3 graphics API.
#   imgui-opengl2: graphics backend for Windows/Linux/MacOS/etc using OpenGL2 graphics API.
#   imgui-vulkan: graphics backend for Windows/Linux/MacOS/etc using Vulkan graphics API.
#   imgui-dx9: graphics backend for Windows using DirectX 9 graphics API.
#   imgui-dx10: graphics backend for Windows using DirectX 10 graphics API.
#   imgui-dx11: graphics backend for Windows using DirectX 11 graphics API.
#   imgui-dx12: graphics backend for Windows using DirectX 12 graphics API.
#   imgui-sdlrenderer: graphics backend for platforms supported by SDL.
#

cmake_minimum_required(VERSION 3.27.6)

# Set a name and a version number for your project:
project(imnodes-playground VERSION 0.0.1 LANGUAGES CXX)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compiler warnings
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wunused-parameter -Wformat -Wnarrowing -Wno-sign-conversion -Wno-error=declaration-after-statement)
    if (NOT APPLE)
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-copy>)
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wunused-parameter -Wformat -Wnarrowing -Wno-sign-conversion -Wno-error=declaration-after-statement -Wnontrivial-memaccess)
    if (NOT APPLE)
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-copy>)
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4)
endif()

# These warnings are overzealous in GCC and only practical to use with Clang.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wshadow)
endif ()



# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_policy(SET CMP0077 NEW)

###############################################################################
# Supported build parameters
###############################################################################
option(IMGUI_EXAMPLES "Build ImGui examples" OFF)
option(IMGUI_DEMO "Include the ImGui demo window implementation in library" OFF)
option(IMGUI_DISABLE_OBSOLETE_FUNCTIONS "Disable deprecated functions" ON)
option(IMGUI_DISABLE_OBSOLETE_KEYIO "Disable legacy input handling" ON)
option(IMGUI_DISABLE_FILE_FUNCTIONS "Disable use of file functions" OFF)
option(IMGUI_ENABLE_STDLIB_SUPPORT "Enable extra functions taking stdlib types" ON)
option(IMGUI_ENABLE_COVERAGE "Enable coverage testing for supported compilers" OFF)
option(IMGUI_ENABLE_ASAN "Enable compiler-based sanitizers" OFF)
option(IMGUI_ENABLE_TRACY "Enable tracy profiling" OFF)
option(IMGUI_ENABLE_FREETYPE "Enable FreeType font rasterizer" OFF)
option(IMGUI_IMPL_SDL "Build the SDL implementation (only if supported)" OFF)
option(IMGUI_IMPL_METAL "Build the Metal implementation (only if supported)" ${APPLE})
option(IMGUI_IMPL_OSX "Build the OSX implementation (only if supported)" ${APPLE})
option(IMGUI_IMPL_WIN32 "Build the Win32 (native winapi) implementation (only if supported)" ${WIN32})
option(IMGUI_IMPL_GLFW "Build the GLFW implementation (only if supported)" ON)
option(IMGUI_IMPL_GLUT "Build the GLUT implementation (only if supported)" ON)
option(IMGUI_IMPL_OPENGL3 "Build the OpenGL 3 implementation (only if supported)" ON)
option(IMGUI_IMPL_OPENGL2 "Build the OpenGL 2 (legacy) implementation (only if supported)" ${IMGUI_IMPL_OPENGL3})
option(IMGUI_IMPL_DX9 "Build the DirectX 9 implementation (only if supported)" ${WIN32})
option(IMGUI_IMPL_DX10 "Build the DirectX 10 implementation (only if supported)" ${WIN32})
option(IMGUI_IMPL_DX11 "Build the DirectX 11 implementation (only if supported)" ${WIN32})
option(IMGUI_IMPL_DX12 "Build the DirectX 12 implementation (only if supported)" ${WIN32})
option(IMGUI_IMPL_VULKAN "Build the Vulkan implementation (only if supported)" OFF)

# set(IMGUI_SDL_TARGET "" CACHE STRING "A custom SDL target name that will be used for linking to examples.")
# set(IMGUI_GLFW_TARGET "" CACHE STRING "A custom GLFW target name that will be used for linking to examples.")
# set(IMGUI_GLUT_TARGET "" CACHE STRING "A custom GLFW target name that will be used for linking to examples.")

###############################################################################

# Tidy up build dir, but only if user has not configured it yet.
if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif ()
if (NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif ()
if (NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif ()

###############################################################################

# this needs to be in the top-level CMakeLists.txt to enable tests
include(CTest)

###############################################################################

# Helper variables.

if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/ext/imgui/imgui.h)
    set (IMGUI_DIR ${CMAKE_CURRENT_LIST_DIR}/ext/imgui CACHE STRING "" FORCE)
endif ()

set (IMGUI_EXAMPLES_DIR ${IMGUI_DIR}/examples CACHE STRING "" FORCE)
set (IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends CACHE STRING "" FORCE)
set (IMGUI_MISC_DIR ${IMGUI_DIR}/misc CACHE STRING "" FORCE)

# Disable warnings for third party source code. We can disable warnings for tracy, because it's source code is built as
# a part of our own build system. In case SDL2/GLFW/FreeType are used from a source distribution, we can not modify
# build flags of those targets from over here and thus they will have a warning spam.
file (GLOB_RECURSE THIRD_PARTY_SOURCE_FILES ${IMGUI_EXAMPLES_DIR}/libs/* imstb_*.c ../tracy/*.cpp)
set_source_files_properties(${THIRD_PARTY_SOURCE_FILES} PROPERTIES COMPILE_OPTIONS -w)

###############################################################################
# Main library
###############################################################################

# Set up main library. It is created as an INTERFACE library because it allows
# using different IMGUI_USER_CONFIG defines in different targets of downstream projects.
file(GLOB IMGUI_SOURCE_FILES ${IMGUI_DIR}/*.h ${IMGUI_DIR}/*.cpp)
if (NOT IMGUI_DEMO)
    list(REMOVE_ITEM IMGUI_SOURCE_FILES ${IMGUI_DIR}/imgui_demo.cpp)
endif ()
add_library(imgui INTERFACE)
target_link_libraries(imgui INTERFACE ${CMAKE_DL_LIBS})
target_sources(imgui INTERFACE ${IMGUI_SOURCE_FILES})
if (IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
    target_compile_definitions(imgui INTERFACE -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1)
endif ()
if (IMGUI_DISABLE_OBSOLETE_KEYIO)
    target_compile_definitions(imgui INTERFACE -DIMGUI_DISABLE_OBSOLETE_KEYIO=1)
endif ()
if (IMGUI_DISABLE_FILE_FUNCTIONS)
    target_compile_definitions(imgui INTERFACE -DIMGUI_DISABLE_FILE_FUNCTIONS=1)
endif ()
if (IMGUI_USER_CONFIG)
    target_compile_definitions(imgui INTERFACE "-DIMGUI_USER_CONFIG=<${IMGUI_USER_CONFIG}>")
endif ()
target_include_directories(imgui INTERFACE ${IMGUI_DIR})
if (NOT IMGUI_DEMO)
    target_compile_definitions(imgui INTERFACE -DIMGUI_DISABLE_DEMO_WINDOWS=1)
endif ()
if (IMGUI_ENABLE_STDLIB_SUPPORT)
    target_include_directories(imgui INTERFACE misc/cpp)
    target_sources(imgui INTERFACE misc/cpp/imgui_stdlib.h misc/cpp/imgui_stdlib.cpp)
endif ()
if (IMGUI_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(imgui INTERFACE -fprofile-arcs -ftest-coverage)
    target_link_libraries(imgui INTERFACE -fprofile-arcs -ftest-coverage)
    target_link_libraries(imgui INTERFACE gcov)
endif ()

if (IMGUI_ENABLE_ASAN AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(imgui INTERFACE -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls)
    target_link_libraries(imgui INTERFACE -g -fsanitize=address)
endif ()

###############################################################################
# Freetype
###############################################################################
if (IMGUI_ENABLE_FREETYPE)
    add_library(imgui-freetype INTERFACE)
    target_sources(imgui-freetype INTERFACE
        ${IMGUI_MISC_DIR}/freetype/imgui_freetype.h
        ${IMGUI_MISC_DIR}/freetype/imgui_freetype.cpp
    )

    find_package(Freetype)
    if (FREETYPE_FOUND)
        target_include_directories(imgui-freetype INTERFACE ${FREETYPE_INCLUDE_DIRS})
        target_link_libraries(imgui-freetype INTERFACE ${FREETYPE_LIBRARIES})
    elseif (EXISTS ../freetype)
        set (SKIP_INSTALL_ALL ON)
        add_subdirectory(../freetype ${CMAKE_BINARY_DIR}/freetype)
        target_link_libraries(imgui-freetype INTERFACE freetype)
    else ()
        message(FATAL_ERROR "IMGUI_ENABLE_FREETYPE but FreeType was not found.")
    endif ()
    target_compile_definitions(imgui-freetype INTERFACE -DIMGUI_ENABLE_FREETYPE=1)
    target_link_libraries(imgui INTERFACE imgui-freetype)
endif()


###############################################################################
# C++ standard library
###############################################################################
add_library(imgui-stdlib INTERFACE)
target_sources(imgui-stdlib INTERFACE
    ${IMGUI_MISC_DIR}/cpp/imgui_stdlib.h
    ${IMGUI_MISC_DIR}/cpp/imgui_stdlib.cpp
)


###############################################################################
# Backends
###############################################################################
# Ignore silently when required headers are missing.
set(CMAKE_REQUIRED_QUIET ON)
include(CheckIncludeFile)

# PkgConfig will be needed for SDL and glfw on some platforms.
find_package(PkgConfig QUIET)

if (IMGUI_IMPL_SDL)
    find_package(SDL2 QUIET)
    if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
        # Nothing to do for emscripten builds. SDL is bunled with the compiler.
    elseif (IMGUI_SDL_TARGET)
        # Custom user target was set. No auto-detection required.
    elseif (EXISTS ${IMGUI_DIR}/../SDL2/CMakeLists.txt)
        # When parent directory contains SDL2 source code - we can build it directly.
        add_subdirectory (${IMGUI_DIR}/../SDL2 ${CMAKE_CURRENT_BINARY_DIR}/SDL2)
        set(IMGUI_SDL_TARGET SDL2-static)
    elseif (TARGET SDL2::SDL2)
        # Some platforms (Linux) have SDL target properly exported as CMake target.
    elseif (SDL2_FOUND)
        # Platforms that do not export target but use old CMake conventions can be handled this way.
        add_library(SDL2::SDL2 INTERFACE IMPORTED)
        target_link_libraries(SDL2::SDL2 INTERFACE ${SDL2_LIBRARIES})
        target_include_directories(SDL2::SDL2 INTERFACE ${SDL2_INCLUDE_DIRS})
    elseif (NOT "$ENV{SDL2_DIR}" STREQUAL "")
        # On windows we may set SDL2_DIR environment variable and link to binary SDL distribution.
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(IMGUI_SDL_PLATFORM_ARCH x64)
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(IMGUI_SDL_PLATFORM_ARCH x86)
        else ()
            message(FATAL_ERROR "Unsupported platform.")
        endif()
        add_library(SDL2::SDL2 SHARED IMPORTED)
        target_include_directories(SDL2::SDL2 INTERFACE "$ENV{SDL2_DIR}/include")
        target_link_libraries(SDL2::SDL2 INTERFACE $ENV{SDL2_DIR}/lib/${IMGUI_SDL_PLATFORM_ARCH}/SDL2main.lib)
        set_target_properties(SDL2::SDL2 PROPERTIES
            IMPORTED_LOCATION "$ENV{SDL2_DIR}/lib/${IMGUI_SDL_PLATFORM_ARCH}/SDL2.dll"
            IMPORTED_IMPLIB "$ENV{SDL2_DIR}/lib/${IMGUI_SDL_PLATFORM_ARCH}/SDL2.lib"
        )
    elseif (PkgConfig_FOUND)
        # Rest of the platforms (like MacOS) can consume SDL via pkg-config.
        # CMake 3.6 supports IMPORTED_TARGET parameter which creates PkgConfig::sdl2 target that we can easily link to.
        # TODO: Switch to using IMPORTED_TARGET when CMake minimal version is increased.
        pkg_check_modules(SDL2 QUIET sdl2)
        if (SDL2_FOUND)
            add_library(SDL2::SDL2 INTERFACE IMPORTED)
            target_link_libraries(SDL2::SDL2 INTERFACE ${SDL2_LDFLAGS})
            target_compile_options(SDL2::SDL2 INTERFACE ${SDL2_CFLAGS})
        endif ()
    endif ()
    if (NOT IMGUI_SDL_TARGET)
        set(IMGUI_SDL_TARGET SDL2::SDL2)
    endif ()
    if (TARGET ${IMGUI_SDL_TARGET} OR CMAKE_SYSTEM_NAME STREQUAL Emscripten)
        # SDL platform
        add_library (imgui-sdl2 INTERFACE)
        target_sources(imgui-sdl2 INTERFACE
            ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl2.h
            ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl2.cpp
        )
        target_include_directories(imgui-sdl2 SYSTEM INTERFACE ${IMGUI_BACKENDS_DIR})
        target_link_libraries(imgui-sdl2 INTERFACE imgui)
        if (TARGET ${IMGUI_SDL_TARGET})
            target_link_libraries(imgui-sdl2 INTERFACE ${IMGUI_SDL_TARGET})
        endif ()

        # SDL renderer
        add_library (imgui-sdlrenderer INTERFACE)
        target_sources(imgui-sdlrenderer INTERFACE
            ${IMGUI_BACKENDS_DIR}/imgui_impl_sdlrenderer.h
            ${IMGUI_BACKENDS_DIR}/imgui_impl_sdlrenderer.cpp
        )
        target_include_directories(imgui-sdlrenderer SYSTEM INTERFACE ${IMGUI_BACKENDS_DIR})
        target_link_libraries(imgui-sdlrenderer INTERFACE imgui)
        if (TARGET ${IMGUI_SDL_TARGET})
            target_link_libraries(imgui-sdlrenderer INTERFACE ${IMGUI_SDL_TARGET})
        endif ()
    else ()
        message(STATUS "IMGUI_IMPL_SDL set to ON but SDL2 could not be found.")
    endif ()
endif ()
if (IMGUI_IMPL_GLFW)
    if (IMGUI_GLFW_TARGET)
        # Custom user target was set. No auto-detection required.
    elseif (TARGET glfw)
        # GLFW exists already. Nothing to do.
    elseif (EXISTS ${IMGUI_DIR}/../glfw/CMakeLists.txt)
        # When parent directory contains GLFW source code - we can build it directly.
        set (GLFW_STANDALONE OFF)
        set (GLFW_INSTALL OFF)
        set (GLFW_BUILD_DOCS OFF)
        add_subdirectory (${IMGUI_DIR}/../glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
    elseif (PkgConfig_FOUND)
        # CMake 3.6 supports IMPORTED_TARGET parameter which creates PkgConfig::glfw target that we can easily link to.
        # TODO: Switch to using IMPORTED_TARGET when CMake minimal version is increased.
        pkg_check_modules(GLFW QUIET glfw3)
        if (GLFW_FOUND)
            add_library(glfw INTERFACE IMPORTED)
            target_link_libraries(glfw INTERFACE ${GLFW_LDFLAGS})
            target_compile_options(glfw INTERFACE ${GLFW_CFLAGS})
        endif ()
    endif ()
    if (NOT IMGUI_GLFW_TARGET)
        set(IMGUI_GLFW_TARGET glfw)
    endif ()
    if (TARGET ${IMGUI_GLFW_TARGET})
        add_library(imgui-glfw INTERFACE)
        target_sources(imgui-glfw INTERFACE
            ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.h
            ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
        )
        target_link_libraries(imgui-glfw INTERFACE ${CMAKE_DL_LIBS} ${IMGUI_GLFW_TARGET})
        target_include_directories(imgui-glfw SYSTEM INTERFACE ${IMGUI_BACKENDS_DIR})
    else ()
        message(STATUS "IMGUI_IMPL_GLFW set to ON but GLFW could not be found.")
    endif ()
endif ()
if (IMGUI_IMPL_OPENGL2 OR IMGUI_IMPL_OPENGL3)
    set(OpenGL_GL_PREFERENCE "GLVND")   # GLVND is vendor-agnostic OpenGL dispatch library on Linux.
    include(FindOpenGL)
    if (NOT OPENGL_FOUND)
        message(FATAL_ERROR "IMGUI_IMPL_OPENGL2 or IMGUI_IMPL_OPENGL3 set to ON but OpenGL could not be found.")
    endif ()
    if (NOT TARGET OpenGL::GL)
        add_library(OpenGL::GL INTERFACE IMPORTED)
        target_include_directories(OpenGL::GL INTERFACE ${OPENGL_INCLUDE_DIR})
        target_link_libraries(OpenGL::GL INTERFACE ${OPENGL_LIBRARY})
    endif ()
    if (IMGUI_IMPL_OPENGL2)
        add_library(imgui-opengl2 INTERFACE)
        target_sources(imgui-opengl2 INTERFACE ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl2.cpp)
        target_link_libraries (imgui-opengl2 INTERFACE imgui OpenGL::GL)
        target_include_directories(imgui-opengl2 SYSTEM INTERFACE ${IMGUI_BACKENDS_DIR})
    endif ()
    if (IMGUI_IMPL_OPENGL3)
        add_library(imgui-opengl3 INTERFACE)
        target_sources(imgui-opengl3 INTERFACE ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp)
        target_link_libraries (imgui-opengl3 INTERFACE imgui OpenGL::GL)
        target_include_directories(imgui-opengl3 SYSTEM INTERFACE ${IMGUI_BACKENDS_DIR})
    endif ()
endif ()

###############################################################################
# imnodes
###############################################################################
if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/ext/imnodes/imnodes.h)
    set (IMNODES_DIR ${CMAKE_CURRENT_LIST_DIR}/ext/imnodes CACHE STRING "" FORCE)
endif ()

add_library(imnodes)
target_sources(imnodes PRIVATE 
    ${IMNODES_DIR}/imnodes.h
    ${IMNODES_DIR}/imnodes_internal.h
    ${IMNODES_DIR}/imnodes.cpp)
target_include_directories(imnodes PUBLIC ${IMNODES_DIR})
target_link_libraries(imnodes PUBLIC imgui)

###############################################################################

# compile the library
# add_subdirectory(src)

# compile the tests
# add_subdirectory(tests)

# compile the application
add_subdirectory(app)

# optionally add doxygen target to generate documentation
option(BUILD_DOCS "Enable building of documentation (requires Doxygen)" OFF)
if(BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    set(DOXYGEN_EXCLUDE_PATTERNS "${CMAKE_SOURCE_DIR}/ext/*")
    doxygen_add_docs(doxygen
        ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

# Optionally enable DEBUG_LOG environment variable
option(DEBUG_LOG "Enable DEBUG_LOG environment variable" OFF)
if (DEBUG_LOG)
    add_compile_definitions(DEBUG_LOG)
endif()
