cmake_minimum_required(VERSION 3.10)
project(verodb C)

# Set C Standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add subdirectories
add_subdirectory(engine)
