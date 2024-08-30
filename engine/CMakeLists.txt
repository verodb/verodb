set(SOURCES
    main.c
    query.c
    storage.c
    aof.c
)

# Create the executable
add_executable(verodb ${SOURCES})

# Include directories for headers
target_include_directories(verodb PRIVATE ${CMAKE_SOURCE_DIR}/engine/query ${CMAKE_SOURCE_DIR}/engine/storage)