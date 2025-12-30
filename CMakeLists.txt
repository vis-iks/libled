cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(libled)

# --- 1. CONFIGURATION SWITCH ---
# Default to OFF (RPI Mode) if we detect an ARM processor, otherwise ON (PC Mode)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    option(X86_BUILD "Build for X86" OFF)
else()
    option(X86_BUILD "Build for X86" ON)
endif()

# Configuration Variables
set(FMOD_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/fmodstudioapi")

if(X86_BUILD)
    add_compile_definitions(X86)
    set(FMOD_ARCH "x86_64")
    message(STATUS "Building for: X86 (Desktop Simulation)")
else()
    add_compile_definitions(RPI)
    # Detect 64-bit Pi vs 32-bit Pi
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(FMOD_ARCH "arm64")
    else()
        set(FMOD_ARCH "arm")
    endif()
    message(STATUS "Building for: Raspberry Pi (${FMOD_ARCH})")
endif()

# --- 2. SETUP FMOD TARGET ---
add_library(fmod_lib SHARED IMPORTED)
set_target_properties(fmod_lib PROPERTIES
    IMPORTED_LOCATION "${FMOD_ROOT}/lib/${FMOD_ARCH}/libfmod.so"
    INTERFACE_INCLUDE_DIRECTORIES "${FMOD_ROOT}/inc"
)

# --- 3. FIND SOURCES ---
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_library(led STATIC ${SOURCES} ${HEADERS})

# --- 4. DEPENDENCIES ---
find_package(X11 REQUIRED)

target_include_directories(led PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/rpi-rgb-led-matrix/include
    ${X11_INCLUDE_DIR}
)

target_link_libraries(led PUBLIC 
    ${X11_LIBRARIES} 
    fmod_lib
    pthread # Matrix lib usually needs threading
)

# --- 5. RPI MATRIX LOGIC ---
if(NOT X86_BUILD)
    message(STATUS "Configuring for Raspberry Pi Hardware...")

    # 1. Find the sources (including .c files!)
    file(GLOB RGB_MATRIX_SOURCES 
        "external/rpi-rgb-led-matrix/lib/*.cc"
        "external/rpi-rgb-led-matrix/lib/*.c"
    )

    # 2. VERIFICATION: Print exactly what we found
    list(LENGTH RGB_MATRIX_SOURCES SRC_COUNT)
    
    if(SRC_COUNT EQUAL 0)
        message(FATAL_ERROR "\n\n"
            "!!! STOP !!!\n"
            "No source files found in 'external/rpi-rgb-led-matrix/lib/'\n"
            "You likely need to run:\n"
            "   git submodule update --init --recursive\n"
            "\n"
        )
    else()
        message(STATUS "-------------------------------------------------")
        message(STATUS "Found ${SRC_COUNT} Matrix files. listing first 3:")
        
        # Print the first few files just to be sure
        list(GET RGB_MATRIX_SOURCES 0 FILE_1)
        message(STATUS "1. ${FILE_1}")
        if(SRC_COUNT GREATER 1)
            list(GET RGB_MATRIX_SOURCES 1 FILE_2)
            message(STATUS "2. ${FILE_2}")
        endif()
        message(STATUS "... (and others)")
        message(STATUS "-------------------------------------------------")
    endif()

    # 3. Add sources and flags
    target_sources(led PRIVATE ${RGB_MATRIX_SOURCES})
    target_compile_definitions(led PUBLIC RPI)
    target_compile_options(led PRIVATE -Wall -O3 -g -fno-strict-aliasing)
    target_link_libraries(led PUBLIC rt m)
endif()

# --- 6. RUNTIME FIX ---
add_custom_command(TARGET led POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FMOD_ROOT}/lib/${FMOD_ARCH}/libfmod.so"
    $<TARGET_FILE_DIR:led>
)

add_subdirectory(demo)