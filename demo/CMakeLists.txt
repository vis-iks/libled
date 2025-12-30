file(GLOB DEMO_SOURCES "*.cpp")

# remove Resources.cpp from sources if file(GLOB) picks it up (it does), 
# but we actually want it. Wait, the glob picks up all .cpp, so Resources.cpp 
# is included.

add_executable(Demo ${DEMO_SOURCES})

# We break the dependency on PandemicCore by using our local Resources.cpp
# However, Resources.cpp might include <Pandemic/core/Resources.h>.
# We copied Resources.h to the local directory, so we need to add '.' to includes.
target_include_directories(Demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Demo PRIVATE led pthread)

# Copy configuration.toml to the build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configuration.toml ${CMAKE_CURRENT_BINARY_DIR}/configuration.toml COPYONLY)
