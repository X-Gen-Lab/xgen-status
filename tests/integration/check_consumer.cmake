# Use a fresh build and installation tree for each real package consumer.
foreach(required XGS_SOURCE_DIR XGS_TEST_ROOT XGS_TEST_CASE XGS_C_COMPILER XGS_GENERATOR)
    if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
        message(FATAL_ERROR "${required} is required")
    endif()
endforeach()
if(NOT XGS_CONFIG)
    set(XGS_CONFIG Debug)
endif()
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef run_id)
set(work "${XGS_TEST_ROOT}/${XGS_TEST_CASE}/${run_id}")
file(MAKE_DIRECTORY "${work}")

function(run_checked step)
    execute_process(COMMAND ${ARGN} RESULT_VARIABLE result
        OUTPUT_VARIABLE output ERROR_VARIABLE errors)
    file(WRITE "${work}/${step}.log" "${output}\n${errors}")
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "${step} failed (${result}); log: ${work}/${step}.log\n${output}\n${errors}")
    endif()
endfunction()

set(generator_args -G "${XGS_GENERATOR}")
if(XGS_GENERATOR_PLATFORM)
    list(APPEND generator_args -A "${XGS_GENERATOR_PLATFORM}")
endif()
if(XGS_GENERATOR_TOOLSET)
    list(APPEND generator_args -T "${XGS_GENERATOR_TOOLSET}")
endif()
set(compiler_args "-DCMAKE_C_COMPILER=${XGS_C_COMPILER}"
    "-DCMAKE_BUILD_TYPE=${XGS_CONFIG}")
set(consumer_args -S "${XGS_SOURCE_DIR}/tests/integration/consumer"
    -B "${work}/consumer" ${generator_args} ${compiler_args}
    "-DCMAKE_CXX_COMPILER=${XGS_CXX_COMPILER}"
    -DXGS_CONSUMER_CHECK_CXX=ON
    -DCMAKE_FIND_USE_PACKAGE_REGISTRY=OFF
    -DCMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY=OFF)

if(XGS_TEST_CASE MATCHES "^source_(.+)$")
    set(mode "${CMAKE_MATCH_1}")
    list(APPEND consumer_args "-DXGS_CONSUMER_SOURCE_DIR=${XGS_SOURCE_DIR}")
elseif(XGS_TEST_CASE MATCHES "^installed_(.+)$")
    set(mode "${CMAKE_MATCH_1}")
    set(build_strings ON)
    if(mode STREQUAL "missing")
        set(build_strings OFF)
        set(mode strings)
    elseif(mode STREQUAL "status_without_strings")
        set(build_strings OFF)
        set(mode status)
        list(APPEND consumer_args -DXGS_CONSUMER_CHECK_CXX=OFF)
    elseif(mode STREQUAL "optional_strings_without_backend")
        set(build_strings OFF)
        set(mode optional_strings)
    endif()
    run_checked(producer_configure "${CMAKE_COMMAND}" -S "${XGS_SOURCE_DIR}"
        -B "${work}/producer" ${generator_args} ${compiler_args}
        -DXGS_BUILD_TESTS=OFF -DXGS_ENABLE_COVERAGE=OFF
        "-DXGS_BUILD_STRINGS=${build_strings}" -DCMAKE_INSTALL_LIBDIR=lib)
    run_checked(producer_build "${CMAKE_COMMAND}" --build "${work}/producer"
        --config "${XGS_CONFIG}")
    run_checked(producer_install "${CMAKE_COMMAND}" --install "${work}/producer"
        --config "${XGS_CONFIG}" --prefix "${work}/prefix")
    if(NOT build_strings AND EXISTS "${work}/prefix/lib/cmake/xgen_status/xgenStatus-strings-targets.cmake")
        message(FATAL_ERROR "Disabled strings were installed")
    endif()
    list(APPEND consumer_args "-Dxgen_status_DIR=${work}/prefix/lib/cmake/xgen_status")
else()
    message(FATAL_ERROR "Unknown test '${XGS_TEST_CASE}'")
endif()
list(APPEND consumer_args "-DXGS_CONSUMER_MODE=${mode}")

if(XGS_TEST_CASE MATCHES "(unknown|wrong_version|missing|preexisting_bad_version|preexisting_bad_abi|preexisting_missing_properties|preexisting_bad_type|preexisting_incomplete)$")
    execute_process(COMMAND "${CMAKE_COMMAND}" ${consumer_args}
        RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors)
    file(WRITE "${work}/expected_failure.log" "${output}\n${errors}")
    if(result EQUAL 0)
        message(FATAL_ERROR "Invalid package request was accepted: ${XGS_TEST_CASE}")
    endif()
    if(XGS_TEST_CASE MATCHES "wrong_version$")
        set(diagnostic "0\\.2\\.0")
    elseif(XGS_TEST_CASE MATCHES "preexisting_bad_type$")
        set(diagnostic "incompatible target type")
    elseif(XGS_TEST_CASE MATCHES "preexisting_incomplete$")
        set(diagnostic "target set is incomplete")
    elseif(XGS_TEST_CASE MATCHES "preexisting_")
        set(diagnostic "incompatible version/ABI")
    else()
        set(diagnostic "is not installed; available:")
    endif()
    if(NOT "${output}\n${errors}" MATCHES "${diagnostic}")
        message(FATAL_ERROR "Failure did not diagnose '${diagnostic}': ${output}\n${errors}")
    endif()
    message(STATUS "Rejected ${XGS_TEST_CASE}; evidence: ${work}")
    return()
endif()

run_checked(consumer_configure "${CMAKE_COMMAND}" ${consumer_args})
run_checked(consumer_build "${CMAKE_COMMAND}" --build "${work}/consumer" --config "${XGS_CONFIG}")
run_checked(consumer_test "${CMAKE_CTEST_COMMAND}" --test-dir "${work}/consumer"
    -C "${XGS_CONFIG}" --output-on-failure --no-tests=error)
message(STATUS "Verified ${XGS_TEST_CASE}; evidence: ${work}")
