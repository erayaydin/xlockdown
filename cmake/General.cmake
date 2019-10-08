# Set policy if available
function(set_policy POL VAL)
    if(POLICY ${POL})
        cmake_policy(SET ${POL} ${VAL})
    endif()
endfunction(set_policy)

function(source_group_by_path PARENT_PATH REGEX GROUP)
    foreach (FILENAME ${ARGN})

        get_filename_component(FILEPATH "${FILENAME}" REALPATH)
        file(RELATIVE_PATH FILEPATH ${PARENT_PATH} ${FILEPATH})
        get_filename_component(FILEPATH "${FILEPATH}" DIRECTORY)

        string(REPLACE "/" "\\" FILEPATH "${FILEPATH}")

        source_group("${GROUP}\\${FILEPATH}" REGULAR_EXPRESSION "${REGEX}" FILES ${FILENAME})

    endforeach()
endfunction(source_group_by_path)

function(list_extract OUTPUT REGEX)
    foreach(FILENAME ${ARGN})
        if(${FILENAME} MATCHES "${REGEX}")
            list(APPEND ${OUTPUT} ${FILENAME})
        endif()
    endforeach()

    set(${OUTPUT} ${${OUTPUT}} PARENT_SCOPE)
endfunction(list_extract)