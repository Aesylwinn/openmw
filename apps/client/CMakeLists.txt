
client_units (.
    client
    clientoptions
)

#set (CLIENT_SRC client.cpp)

add_executable (openmw-client
    main.cpp
    ${CLIENT_SRC}
)

target_link_libraries (openmw-client
    ${OSG_LIBRARIES}
    ${OSGVIEWER_LIBRARIES}
    ${SDL2_LIBRARY}
    components
)

add_custom_command(TARGET openmw-client
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openmw-client> ${PROJECT_BINARY_DIR}
)
