function(enable_unity_build UB_SUFFIX SOURCE_VARIABLE_NAME)
    set(files ${SOURCE_VARIABLE_NAME})
    # Generate a unique filename for the unity build translation unit
    set(unit_build_file ${CMAKE_CURRENT_BINARY_DIR}/ub_${UB_SUFFIX}.cpp)
    # Exclude all translation units from compilation
    set_source_files_properties(${files} PROPERTIES HEADER_FILE_ONLY true)
    # Open the ub file
        FILE(WRITE ${unit_build_file} "// Unity Build generated by CMake\n")
    # Add include statement for each translation unit
    foreach(source_file ${files} )
        FILE( APPEND ${unit_build_file} "#include <${source_file}>\n")
    endforeach(source_file)
    # Complement list of translation units with the name of ub
    set(${SOURCE_VARIABLE_NAME} ${${SOURCE_VARIABLE_NAME}} ${unit_build_file} PARENT_SCOPE)
endfunction(enable_unity_build)



macro (add_openmw_dir dir)
    set (files)
    set (cppfiles)
    foreach (u ${ARGN})

        # Add cpp and hpp to OPENMW_FILES
        file (GLOB ALL "${dir}/${u}.[ch]pp")
        foreach (f ${ALL})
            list (APPEND files "${f}")
            list (APPEND OPENMW_FILES "${f}")
        endforeach (f)

        # Add cpp to unity build
        file (GLOB ALL "${dir}/${u}.cpp")
        foreach (f ${ALL})
            list (APPEND cppfiles "${f}")
        endforeach (f)

    endforeach (u)

    if (OPENMW_UNITY_BUILD)
        enable_unity_build(${dir} "${cppfiles}")
        list (APPEND OPENMW_FILES ${CMAKE_CURRENT_BINARY_DIR}/ub_${dir}.cpp)
    endif()

    source_group ("apps\\openmw\\${dir}" FILES ${files})
endmacro (add_openmw_dir)

macro (add_component_dir dir)
    set (files)
    set (cppfiles)

    foreach (u ${ARGN})
        file (GLOB ALL "${dir}/${u}.[ch]pp")

        foreach (f ${ALL})
            list (APPEND files "${f}")
            list (APPEND COMPONENT_FILES "${f}")
        endforeach (f)

        # Add cpp to unity build
        file (GLOB ALL "${dir}/${u}.cpp")
        foreach (f ${ALL})
            list (APPEND cppfiles "${f}")
        endforeach (f)

    endforeach (u)

    if (OPENMW_UNITY_BUILD)
        enable_unity_build(${dir} "${cppfiles}")
        list (APPEND COMPONENT_FILES ${CMAKE_CURRENT_BINARY_DIR}/ub_${dir}.cpp)
    endif()

    source_group ("components\\${dir}" FILES ${files})
endmacro (add_component_dir)

macro (add_component_qt_dir dir)
set (files)
foreach (u ${ARGN})
file (GLOB ALL "${dir}/${u}.[ch]pp")
foreach (f ${ALL})
list (APPEND files "${f}")
list (APPEND COMPONENT_FILES "${f}")
endforeach (f)
file (GLOB MOC_H "${dir}/${u}.hpp")
foreach (fi ${MOC_H})
list (APPEND COMPONENT_MOC_FILES "${fi}")
endforeach (fi)
endforeach (u)
source_group ("components\\${dir}" FILES ${files})
endmacro (add_component_qt_dir)

macro (copy_all_files source_dir destination_dir files)
foreach (f ${files})
get_filename_component(filename ${f} NAME)
configure_file(${source_dir}/${f} ${destination_dir}/${filename} COPYONLY)
endforeach (f)
endmacro (copy_all_files)

macro (add_file project type file)
list (APPEND ${project}${type} ${file})
endmacro (add_file)

macro (add_unit project dir unit)
add_file (${project} _HDR ${comp} "${dir}/${unit}.hpp")
add_file (${project} _SRC ${comp} "${dir}/${unit}.cpp")
endmacro (add_unit)

macro (add_qt_unit project dir unit)
add_file (${project} _HDR ${comp} "${dir}/${unit}.hpp")
add_file (${project} _HDR_QT ${comp} "${dir}/${unit}.hpp")
add_file (${project} _SRC ${comp} "${dir}/${unit}.cpp")
endmacro (add_qt_unit)

macro (add_hdr project dir unit)
add_file (${project} _HDR ${comp} "${dir}/${unit}.hpp")
endmacro (add_hdr)

macro (add_qt_hdr project dir unit)
add_file (${project} _HDR ${comp} "${dir}/${unit}.hpp")
add_file (${project} _HDR_QT ${comp} "${dir}/${unit}.hpp")
endmacro (add_qt_hdr)

macro (opencs_units dir)
foreach (u ${ARGN})
add_qt_unit (OPENCS ${dir} ${u})
endforeach (u)
endmacro (opencs_units)

macro (opencs_units_noqt dir)
foreach (u ${ARGN})
add_unit (OPENCS ${dir} ${u})
endforeach (u)
endmacro (opencs_units_noqt)

macro (opencs_hdrs dir)
foreach (u ${ARGN})
add_qt_hdr (OPENCS ${dir} ${u})
endforeach (u)
endmacro (opencs_hdrs)

macro (opencs_hdrs_noqt dir)
foreach (u ${ARGN})
add_hdr (OPENCS ${dir} ${u})
endforeach (u)
endmacro (opencs_hdrs_noqt)

macro (client_units dir)
foreach (u ${ARGN})
add_unit (CLIENT ${dir} ${u})
endforeach (u)
endmacro (client_units)

macro (client_hdrs dir)
foreach (u ${ARGN})
add_hdr (CLIENT ${dir} ${u})
endforeach (u)
endmacro (client_hdrs)
