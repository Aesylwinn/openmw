
add_executable (openmw-server
    main.cpp
)

target_link_libraries (openmw-server
    components
)

add_custom_command(TARGET openmw-server
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openmw-server> ${PROJECT_BINARY_DIR}
)

