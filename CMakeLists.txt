cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(libled)

set(X86_BUILD ON) 
add_compile_definitions(X86)

# --- CONFIGURATION ---
set(FMOD_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/fmodstudioapi")

# --- 1. SETUP FMOD TARGET ---
add_library(fmod_lib SHARED IMPORTED)
set_target_properties(fmod_lib PROPERTIES
    IMPORTED_LOCATION "${FMOD_ROOT}/core/lib/x86_64/libfmod.so"
    INTERFACE_INCLUDE_DIRECTORIES "${FMOD_ROOT}/core/inc"
)

# --- 2. FIND SOURCES ---
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_library(led STATIC ${SOURCES} ${HEADERS})

# --- 3. DEPENDENCIES ---
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
)

# --- 4. RPI MATRIX LOGIC ---
if(NOT X86_BUILD)
    file(GLOB RGB_MATRIX_SOURCES "external/rpi-rgb-led-matrix/lib/*.cc")
    target_sources(led PRIVATE ${RGB_MATRIX_SOURCES})
    target_compile_definitions(led PUBLIC RPI)
endif()

# --- 5. RUNTIME FIX ---
add_custom_command(TARGET led POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FMOD_ROOT}/core/lib/x86_64/libfmod.so"
    $<TARGET_FILE_DIR:led>
)

add_subdirectory(demo)