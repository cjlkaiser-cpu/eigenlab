cmake_minimum_required(VERSION 3.16)
project(eigenlab-core VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
set(SOURCES
    src/core/types.cpp
    src/physics/particle_system.cpp
    src/physics/heat_diffusion.cpp
    src/physics/fluid_solver.cpp
    src/physics/boids.cpp
    src/physics/galaxy.cpp
    src/physics/galaxy_collision.cpp
    src/physics/erosion.cpp
    src/physics/softbody.cpp
    src/physics/wave_solver.cpp
    src/physics/molecular_dynamics.cpp
    src/physics/lattice_boltzmann.cpp
    src/physics/granular_sim.cpp
    src/physics/neural_network.cpp
    src/physics/accretion_disk.cpp
    src/physics/cellular_automata_3d.cpp
    src/physics/mandelbulb.cpp
    src/physics/electrophoresis.cpp
    src/physics/plasma_particles.cpp
    src/bindings.cpp
)

# Header files (for IDE integration)
set(HEADERS
    include/core/types.hpp
    include/physics/particle_system.hpp
    include/physics/heat_diffusion.hpp
)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Create the library/executable
if(EMSCRIPTEN)
    # WebAssembly build
    add_executable(eigenlab-core ${SOURCES} ${HEADERS})

    # Emscripten-specific flags
    set_target_properties(eigenlab-core PROPERTIES
        OUTPUT_NAME "eigenlab-core"
        SUFFIX ".js"
    )

    # Compiler flags
    target_compile_options(eigenlab-core PRIVATE
        -O3                     # Maximum optimization
        -ffast-math             # Aggressive floating-point optimization
        -fno-exceptions         # Disable exceptions for smaller code
        -flto                   # Link-time optimization
    )

    # Linker flags
    target_link_options(eigenlab-core PRIVATE
        "SHELL:-s WASM=1"
        "SHELL:-s MODULARIZE=1"
        "SHELL:-s EXPORT_NAME='EigenLabCore'"
        "SHELL:-s ENVIRONMENT='web'"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:-s INITIAL_MEMORY=16777216"  # 16MB initial
        "SHELL:-s MAXIMUM_MEMORY=268435456" # 256MB max
        "SHELL:-s NO_EXIT_RUNTIME=1"
        "SHELL:-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
        "SHELL:--bind"                       # Enable Embind
        "SHELL:-flto"
        "SHELL:-O3"
    )

else()
    # Native build (for testing)
    add_library(eigenlab-core STATIC ${SOURCES} ${HEADERS})

    target_compile_options(eigenlab-core PRIVATE
        -O3
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# Installation
install(TARGETS eigenlab-core
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)
