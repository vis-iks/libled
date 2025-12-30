cmake_minimum_required(VERSION 3.10)
project(libled)

set(CMAKE_CXX_STANDARD 17)
set(X86_BUILD ON) # Explicitly set variable for CMake logic
add_compile_definitions(X86)

# Find sources
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_library(led STATIC ${SOURCES} ${HEADERS})

# Find X11
find_package(X11 REQUIRED)

# Add include directories
# Exclude rpi-rgb-led-matrix sources from the main glob to avoid compilation errors on non-RPI
# We will add them manually if needed, or rely on the RPI guard
list(FILTER SOURCES EXCLUDE REGEX "external/rpi-rgb-led-matrix/.*")

# Add include directories
target_include_directories(led PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/fmodstudioapi20311linux/api/core/inc
    ${X11_INCLUDE_DIR}
)

# Link FMOD
target_link_directories(led PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/fmodstudioapi20311linux/api/core/lib/x86_64)
target_link_libraries(led PUBLIC ${X11_LIBRARIES} fmod)

# Flag to determine if we are building for RPI (usually set by toolchain, but we can infer or let user set it)
# For this demo environment, we are on X86, so we don't compile the matrix lib.
# HOWEVER, the user explicitly asked to "include the rgbmatrix library" so they can "get rid of the warning".
# The warning comes from the root CMakeLists.txt finding the library.
# We will compile the library as a static helper if requested, or just rely on the headers being present to satisfy includes.
# Since DotMatrixGraphics.cpp is guarded by #ifdef RPI, we don't strictly need to link the library on X86.
# But if the user wants to compile *everything* including the matrix lib (even if unused on X86), we can try.
# Note: The matrix lib has hardware dependencies (GPIO). It won't compile on standard X86 without special flags (RGB_LIBRARY_V4L2_REAL_TIME_REGISTERS etc).
# Safest bet: Just add the include path so headers are found, and only compile sources if RPI is defined.
# If the user wants to remove the warning, we should fix root CMakeLists.txt.

# To satisfy the user's specific request "include it as submodule... then I can get rid of the library not found"
# I will define a target for it if RPI is set, or if we want to simulate.
# But simply having the submodule doesn't fix the "find_library" call in root CMake. That needs to be removed/changed.

# Let's add the sources to 'led' library ONLY if we seem to be on RPi or if forced.
if(NOT X86_BUILD)
    file(GLOB RGB_MATRIX_SOURCES "external/rpi-rgb-led-matrix/lib/*.cc")
    target_sources(led PRIVATE ${RGB_MATRIX_SOURCES})
    target_compile_definitions(led PUBLIC RPI)
endif()

add_subdirectory(demo)

