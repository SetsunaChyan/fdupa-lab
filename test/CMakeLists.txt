add_custom_target(unit_tests)
set(TESTCASES_DIR ${PROJECT_ROOT_DIR}/testcases)
add_definitions(-DTESTCASES_DIR=\"${TESTCASES_DIR}\")

file(GLOB_RECURSE SOURCES *.cpp)

function(add_unit_test test_name)
    message("Set up unit test: ${test_name}")
    get_filename_component(test ${test_name} NAME_WE)
    message("test: ${test}")
    add_executable(${test} ${test_name})
    add_dependencies(unit_tests ${test})

    target_link_libraries(${test} LINK_PUBLIC gtest_main gtest fdupa)

    add_test(NAME "${test}"
        COMMAND ${test} ${CATCH_TEST_FILTER}
        WORKING_DIRECTORY ${PA_UNITTEST_DIR}
    )

    set_tests_properties("${test}" PROPERTIES LABELS "all")
    set(CTEST_OUTPUT_ON_FAILURE ON)
endfunction()

foreach(src ${SOURCES})
    add_unit_test(${src})
endforeach(src)
