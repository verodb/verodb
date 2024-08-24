cmake_minimum_required(VERSION 3.10)
project(verodb C)

# Set C Standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add subdirectories
add_subdirectory(engine)

# Find Cppcheck
find_program(CPPCHECK cppcheck)

# Add Cppcheck custom target if Cppcheck is found
if(CPPCHECK)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK} --enable=all --inconclusive --std=c99 --force ${CMAKE_SOURCE_DIR}/engine
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running Cppcheck"
        VERBATIM
    )
endif()
