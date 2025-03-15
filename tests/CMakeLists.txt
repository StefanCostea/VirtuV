enable_testing()

#grab all basic directed test files and add them individually
file(GLOB BASIC_DIRECTED_PY_TESTS "${CMAKE_SOURCE_DIR}/tests/basic_directed/*.py")

foreach(test_file ${BASIC_DIRECTED_PY_TESTS})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_test(NAME basic_directed_${test_name}
             COMMAND ${Python3_EXECUTABLE} ${test_file})
endforeach()
