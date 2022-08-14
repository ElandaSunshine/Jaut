########################################################################################################################
# Include dataset processor
include(${PROJECT_SOURCE_DIR}/cmake/CsvParser.cmake)



########################################################################################################################
# Helpers
macro(jaut_get_dataset name url)
    cmake_parse_arguments(PARG "REQUIRED;NO_HEADERS" "" "" ${ARGN})

    message(STATUS "[Jaut] Dataset: ${name} (${url})")
    file(DOWNLOAD "${url}" ${CMAKE_CURRENT_LIST_DIR}/${name}.csv
        STATUS op_status
        SHOW_PROGRESS)

    list(GET op_status 0 op_code)
    list(GET op_status 1 op_message)


    if (${op_code} GREATER 1)
        if (${PARG_REQUIRED})
            message(FATAL_ERROR "[Jaut] Dataset '${name}' couldn't be processed: ${op_message}")
        endif()

        message(ERROR "[Jaut] Dataset '${name}' couldn't be processed, skipping: ${op_message}")
    endif()

    set(IGNORE_HEADER "")

    if (${PARG_NO_HEADERS})
        set(IGNORE_HEADER "HEADER IGNORE")
    endif()

    message(STATUS "[Jaut] Parsing document...")
    csv(IMPORT ${name}
        FILE "${CMAKE_CURRENT_LIST_DIR}/${name}.csv"
        HEADER IGNORE
        STRICT)

    message(STATUS "[Jaut] Finished for '${name}'")
endmacro()

function(jaut_create_array_string document)
    list(LENGTH ${document}_DOCUMENT length)

    foreach(i RANGE 1 ${length})
        set(record "${${document}_RECORD_${i}}")

        string(REPLACE "\;" "\\\;" record "${record}")
        list(TRANSFORM record STRIP)

        string(REPLACE "\;" "\\\;" record "${record}")
        list(TRANSFORM record REPLACE "(.+)" "\"\\1\"")

        list(JOIN record ", " output_entries)

        string(REPLACE "\;" ";" output_entries "${output_entries}")
        string(APPEND output "        { ${output_entries} }")

        if (${i} LESS ${length})
            string(APPEND output ",\n")
        endif()
    endforeach()

    set(JAUT_DATASET_${document}     "${output}" PARENT_SCOPE)
    set(JAUT_DATASET_${document}_LEN "${length}" PARENT_SCOPE)
endfunction()



########################################################################################################################
# Prepare datasets
message(STATUS "[Jaut] Fetching datasets")
jaut_get_dataset(languages "https://datahub.io/core/language-codes/r/language-codes-3b2.csv"
    REQUIRED
    NO_HEADERS)
csv(MOVE COLUMN languages 1
    RIGHT)

jaut_get_dataset(countries "https://gist.githubusercontent.com/tadast/8827699/raw/f5cac3d42d16b78348610fc4ec301e9234f82821/countries_codes_and_coordinates.csv"
    REQUIRED
    NO_HEADERS)
csv(REMOVE countries
    COLUMN 6)
csv(REMOVE countries
    COLUMN 5)
csv(REMOVE countries
    COLUMN 4)
csv(MOVE COLUMN countries 1
    RIGHT 2)

# Read IANA entries
file(DOWNLOAD "https://www.iana.org/assignments/language-subtag-registry/language-subtag-registry"
    "${CMAKE_CURRENT_LIST_DIR}/language-subtag-registry.txt")
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/language-subtag-registry.txt" IANA_LINES)

csv(CREATE scripts)
csv(INSERT COLUMN scripts)
csv(INSERT COLUMN scripts)

set(CURRENT_IANA_TYPE "")
set(CURRENT_IANA_TAG  "")
set(CURRENT_IANA_DESC "")

foreach(line IN LISTS IANA_LINES)
    if ("${line}" STREQUAL "%%")
        if ("${CURRENT_IANA_TYPE}" STREQUAL "script")
            string(REPLACE ";" "\;" CURRENT_IANA_DESC "${CURRENT_IANA_DESC}")
            csv(INSERT RECORD scripts VALUES "${CURRENT_IANA_TAG}" "${CURRENT_IANA_DESC}")
        endif()

        set(CURRENT_IANA_TYPE "")
        set(CURRENT_IANA_TAG  "")
        set(CURRENT_IANA_DESC "")

        continue()
    endif()

    if ("${line}" MATCHES "^Type:[ ]*([^ ]*)")
        set(CURRENT_IANA_TYPE "${CMAKE_MATCH_1}")
    endif()

    if ("${line}" MATCHES "^Subtag:[ ]*(.*)")
        list(APPEND CURRENT_IANA_TAG "${CMAKE_MATCH_1}")
    endif()

    if ("${line}" MATCHES "^Description:[ ]*(.*)")
        string(REPLACE ";" "\;" CMAKE_MATCH_1 "${CMAKE_MATCH_1}")
        list(APPEND CURRENT_IANA_DESC "${CMAKE_MATCH_1}")
    endif()
endforeach()



########################################################################################################################
# Generating header for datasets
message(STATUS "[Jaut] Generating 'jaut_Datasets.h' at '${CMAKE_CURRENT_LIST_DIR}/include'")

# Create arrays
jaut_create_array_string(languages)
jaut_create_array_string(countries)
jaut_create_array_string(scripts)

# Generate header
configure_file("${CMAKE_CURRENT_LIST_DIR}/jaut_dataset_locale.h.in"
    "${CMAKE_CURRENT_LIST_DIR}/include/jaut_datasets/jaut_dataset_locale.h"
    @ONLY)
