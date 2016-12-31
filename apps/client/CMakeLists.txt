
add_executable (openmw-client
    main.cpp
)

target_link_libraries (openmw-client
    components
    ${Bullet_LIBRARIES}
)

add_custom_command(TARGET openmw-client
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openmw-client> ${PROJECT_BINARY_DIR}
)
