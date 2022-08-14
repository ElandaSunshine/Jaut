########################################################################################################################
#span(MAKE <string-var> <start> <end> <out-var>)
#span(GET <span> <out-var>)
function(span operation)
    if ("${operation}" STREQUAL "MAKE")
        if (${ARGC} LESS 5)
            message(FATAL_ERROR "Too few arguments to operation 'MAKE': span(MAKE <input> <start> <end> <out-var>)")
        endif()

        set(text  "${${ARGV1}}")
        set(start ${ARGV2})
        set(end   ${ARGV3})
        set(var   "${ARGV4}")

        string(LENGTH "${text}" in_len)

        if (${in_len} EQUAL 0)
            set(${var} "0:0:${ARGV1}" PARENT_SCOPE)
            return()
        endif()

        if (${start} LESS 0 OR ${start} GREATER ${in_len})
            message(FATAL_ERROR "Start index out of bounds")
        endif()

        if (${end} LESS 0 OR ${end} GREATER ${in_len})
            message(FATAL_ERROR "End index out of bounds")
        endif()

        if (NOT ${start} LESS ${end})
            message(FATAL_ERROR "End index must be greater than start index")
        endif()

        set(${var} "${start}:${end}:${ARGV1}" PARENT_SCOPE)
    elseif("${operation}" STREQUAL "GET")
        if (${ARGC} LESS 3)
            message(FATAL_ERROR "Too few arguments to operation 'GET': span(GET <span> <out-var>)")
        endif()

        set(span "${ARGV1}")
        set(var  "${ARGV2}")

        string(FIND "${${span}}" ":" span_sep_pos)

        if (${span_sep_pos} EQUAL -1)
            message(FATAL_ERROR "Span '${span}' is not a valid span")
        endif()

        string(SUBSTRING "${${span}}" 0 ${span_sep_pos} span_pos_1)

        if (NOT "${span_pos_1}" MATCHES "[0]|[1-9][0-9]*")
            message(FATAL_ERROR "Span '${span}' is not a valid span")
        endif()

        math(EXPR span_sep_pos "${span_sep_pos} + 1")
        string(SUBSTRING "${${span}}" ${span_sep_pos} -1 span_remainder)

        string(FIND "${span_remainder}" ":" span_sep_pos)

        if (${span_sep_pos} EQUAL -1)
            message(FATAL_ERROR "Span '${span}' is not a valid span")
        endif()

        string(SUBSTRING "${span_remainder}" 0 ${span_sep_pos} span_pos_2)

        if (NOT "${span_pos_2}" MATCHES "[0]|[1-9][0-9]*")
            message(FATAL_ERROR "Span '${span}' is not a valid span")
        endif()

        math(EXPR span_sep_pos "${span_sep_pos} + 1")
        string(SUBSTRING "${span_remainder}" ${span_sep_pos} -1 span_content_var)

        if (NOT DEFINED ${span_content_var})
            message(FATAL_ERROR "Span '${span}' object not valid")
        endif()

        if (${span_pos_1} EQUAL 0 AND ${span_pos_2} EQUAL 0)
            set(${var} "" PARENT_SCOPE)
            return()
        endif()

        math(EXPR sub_len "${span_pos_2} - ${span_pos_1}")

        string(SUBSTRING "${${span_content_var}}" ${span_pos_1} ${sub_len} span_text)
        set(${var} "${span_text}" PARENT_SCOPE)
    endif()
endfunction()
