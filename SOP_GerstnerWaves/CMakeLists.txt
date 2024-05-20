cmake_minimum_required( VERSION 3.6 )

project( HDK_GerstnerWaveGenerator )

# CMAKE_PREFIX_PATH must contain the path to the toolkit/cmake subdirectory of
# the Houdini installation. See the "Compiling with CMake" section of the HDK
# documentation for more details, which describes several options for
# specifying this path.
list( APPEND CMAKE_PREFIX_PATH "$ENV{HFS}/toolkit/cmake" )

# Locate Houdini's libraries and header files.
# Registers an imported library target named 'Houdini'.
find_package( Houdini REQUIRED )

set( library_name GerstnerWaveGenerator )



# Add a library and ierstnerWaveGenerator.cpp
add_library( ${library_name} SHARED
	GerstnerWaveGenerator.cpp
	GerstnerWaveGenerator.h
	Names.h
	GerstnerWave.h
	ParallelAdditionEvaluator.h
	ParallelAdditionEvaluator.cpp
	LocalGerstnerWave.h
	CircularGerstnerWave.h

)

# Link against the Houdini libraries, and add required include directories and
# compile definitions.
target_link_libraries( ${library_name} Houdini )

# Include ${CMAKE_CURRENT_BINARY_DIR} for the generated header.
target_include_directories( ${library_name} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Sets several common target properties, such as the library's output directory.
houdini_configure_target( ${library_name} )
