file(GLOB libs "${OUTDIR}/lib*.a")
list(FILTER libs EXCLUDE REGEX ".*/lib(gtest|gmock).*")
list(FILTER libs EXCLUDE REGEX ".*/libmdict-fat\\.a$")

set(merge_dir "${ROOT}/merge")
file(REMOVE_RECURSE "${merge_dir}")
file(MAKE_DIRECTORY "${merge_dir}")

foreach(lib IN LISTS libs)
    execute_process(
        COMMAND "${AR}" x "${lib}"
        WORKING_DIRECTORY "${merge_dir}"
        RESULT_VARIABLE rv
    )
    if(NOT rv EQUAL 0)
        message(FATAL_ERROR "extract failed: ${lib}")
    endif()
endforeach()

file(GLOB objs "${merge_dir}/*.o")
if(NOT objs)
    message(FATAL_ERROR "no objects extracted")
endif()

execute_process(
    COMMAND "${AR}" rcs "${OUTDIR}/libmdict-fat.a" ${objs}
    WORKING_DIRECTORY "${merge_dir}"
    RESULT_VARIABLE rv
)
if(NOT rv EQUAL 0)
    message(FATAL_ERROR "archive failed")
endif()

file(REMOVE_RECURSE "${merge_dir}")

file(REMOVE
    "${OUTDIR}/libmdict.a"
    "${OUTDIR}/libmdictminiz.a"
    "${OUTDIR}/libmdictbase64.a"
    "${OUTDIR}/libminiz.a"
    "${OUTDIR}/libbase64.a"
)

file(RENAME "${OUTDIR}/libmdict-fat.a" "${OUTDIR}/libmdict.a")