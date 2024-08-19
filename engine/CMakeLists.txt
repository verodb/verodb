# Specify source files for the executable
set(SOURCES
    main.c
    query/query.c
    storage/storage.c
)

# Create the executable
add_executable(verodb ${SOURCES})

# Include directories for headers
target_include_directories(verodb PRIVATE ${CMAKE_SOURCE_DIR}/engine/query ${CMAKE_SOURCE_DIR}/engine/storage)
