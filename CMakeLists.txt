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
add_subdirectory(storage)

add_executable(verodb main.c)
target_link_libraries(verodb schema indexing integration security utils storage)

target_include_directories(verodb PRIVATE 
    ${PROJECT_SOURCE_DIR}/schema/src
    ${PROJECT_SOURCE_DIR}/indexing/src
    ${PROJECT_SOURCE_DIR}/integration/src
    ${PROJECT_SOURCE_DIR}/security/src
    ${PROJECT_SOURCE_DIR}/utils/src
    ${PROJECT_SOURCE_DIR}/storage/src
)
