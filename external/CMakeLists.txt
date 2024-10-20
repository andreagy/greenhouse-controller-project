# Add external sources
target_sources(${PROJECT_NAME} PRIVATE
    nanomodbus/nanomodbus.c
)

# Define include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)