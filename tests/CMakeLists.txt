include(GoogleTest)

add_executable(xgs_tests unit/test_status.cpp)
set_target_properties(xgs_tests PROPERTIES
    C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_link_libraries(xgs_tests PRIVATE xgs::status GTest::gtest_main)
if(XGS_BUILD_STRINGS)
    target_sources(xgs_tests PRIVATE support/status_from_int.c)
    target_link_libraries(xgs_tests PRIVATE xgs::strings)
endif()
target_compile_definitions(xgs_tests PRIVATE XGS_TEST_STRINGS=$<BOOL:${XGS_BUILD_STRINGS}>)
xgs_enable_warnings(xgs_tests)
gtest_discover_tests(xgs_tests DISCOVERY_MODE POST_BUILD
    PROPERTIES LABELS "unit" TIMEOUT 30)

add_library(xgs_header_checks OBJECT
    integration/include_status.c integration/include_status.cpp
    integration/include_version.c integration/include_version.cpp)
set_target_properties(xgs_header_checks PROPERTIES
    C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_link_libraries(xgs_header_checks PRIVATE xgs::status)
xgs_enable_warnings(xgs_header_checks)

foreach(test_case source_status source_strings source_preexisting_compatible
        source_preexisting_bad_version source_preexisting_bad_abi
        source_preexisting_missing_properties source_preexisting_bad_type
        installed_status installed_strings installed_default installed_optional
        installed_status_without_strings installed_unknown installed_wrong_version
        installed_missing installed_preexisting_compatible
        installed_preexisting_bad_version installed_preexisting_bad_abi
        installed_preexisting_missing_properties installed_preexisting_bad_type
        source_strings_preexisting_compatible installed_strings_preexisting_compatible
        source_strings_preexisting_bad_abi installed_strings_preexisting_bad_abi
        source_preexisting_incomplete installed_optional_strings_without_backend)
    add_test(NAME xgs_${test_case}
        COMMAND "${CMAKE_COMMAND}"
            "-DXGS_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
            "-DXGS_TEST_ROOT=${PROJECT_BINARY_DIR}/integration"
            "-DXGS_TEST_CASE=${test_case}"
            "-DXGS_C_COMPILER=${CMAKE_C_COMPILER}"
            "-DXGS_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DXGS_GENERATOR=${CMAKE_GENERATOR}"
            "-DXGS_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DXGS_GENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DXGS_CONFIG=$<CONFIG>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/integration/check_consumer.cmake")
    set_tests_properties(xgs_${test_case} PROPERTIES LABELS "integration" TIMEOUT 120)
endforeach()
