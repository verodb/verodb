cmake_minimum_required(VERSION 3.10)
project(verodb C)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Add subdirectories for each module
add_subdirectory(schema)
add_subdirectory(indexing)
add_subdirectory(integration)
add_subdirectory(security)
add_subdirectory(utils)

# Add the main executable
add_executable(verodb main.c)

# Link the modules to the main executable
target_link_libraries(verodb schema indexing integration security utils)
