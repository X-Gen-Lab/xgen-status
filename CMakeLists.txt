cmake_minimum_required(VERSION 3.24)
project(xgen_status VERSION 0.1.0 DESCRIPTION "Independent C11 status codes and optional strings" LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(cmake/xgenStatusCompatibility.cmake)

option(XGS_BUILD_STRINGS "Build the optional diagnostic string implementation" ON)
option(XGS_BUILD_TESTS "Build host GoogleTest and package consumption tests" OFF)
option(XGS_ENABLE_COVERAGE "Instrument owned production objects for host coverage" OFF)

if(XGS_ENABLE_COVERAGE)
    if(CMAKE_CROSSCOMPILING OR NOT XGS_BUILD_TESTS OR NOT XGS_BUILD_STRINGS)
        message(FATAL_ERROR "Coverage requires an explicit host test build with strings")
    endif()
    if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$" OR
       CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        message(FATAL_ERROR "Coverage requires a GCC-compatible host compiler")
    endif()
endif()

configure_file(include/xgen/status/version.h.in generated/xgen/status/version.h @ONLY)

if(TARGET xgs::status OR TARGET xgs::strings)
    set(required_components status)
    if(XGS_BUILD_STRINGS)
        list(APPEND required_components strings)
    endif()
    foreach(component IN LISTS required_components)
        if(NOT TARGET xgs::${component})
            message(FATAL_ERROR "Supplied xgen-status target set is incomplete: xgs::${component} is missing")
        endif()
        set(expected_type INTERFACE_LIBRARY)
        if(component STREQUAL "strings")
            set(expected_type STATIC_LIBRARY)
        endif()
        xgs_check_target(xgs::${component} "${expected_type}" "${PROJECT_VERSION}" compatibility_error)
        if(compatibility_error)
            message(FATAL_ERROR "${compatibility_error}")
        endif()
    endforeach()
    # The original provider owns installation and build options for its targets.
    return()
endif()

function(xgs_enable_warnings target)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow
            $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>)
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    endif()
endfunction()

function(xgs_export_component component)
    set_target_properties(xgs_${component} PROPERTIES
        EXPORT_NAME ${component}
        XGS_VERSION ${PROJECT_VERSION}
        XGS_ABI_VERSION 1
        EXPORT_PROPERTIES "XGS_VERSION;XGS_ABI_VERSION")
    install(TARGETS xgs_${component} EXPORT xgenStatus${component}Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(EXPORT xgenStatus${component}Targets
        FILE xgenStatus-${component}-targets.cmake NAMESPACE xgs::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xgen_status)
endfunction()

add_library(xgs_status INTERFACE)
add_library(xgs::status ALIAS xgs_status)
target_compile_features(xgs_status INTERFACE c_std_11)
target_include_directories(xgs_status INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
xgs_export_component(status)

set(XGS_COMPONENTS status)
if(XGS_BUILD_STRINGS)
    add_library(xgs_strings STATIC src/status.c)
    add_library(xgs::strings ALIAS xgs_strings)
    set_target_properties(xgs_strings PROPERTIES
        C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(xgs_strings PUBLIC xgs::status)
    xgs_enable_warnings(xgs_strings)
    if(XGS_ENABLE_COVERAGE)
        target_compile_options(xgs_strings PRIVATE --coverage -O0 -g)
        target_link_options(xgs_strings INTERFACE --coverage)
    endif()
    xgs_export_component(strings)
    list(APPEND XGS_COMPONENTS strings)
endif()

install(FILES include/xgen/status/status.h
    "${CMAKE_CURRENT_BINARY_DIR}/generated/xgen/status/version.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xgen/status)
configure_package_config_file(cmake/xgen_statusConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/xgen_statusConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xgen_status)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/xgen_statusConfigVersion.cmake"
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/xgen_statusConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/xgen_statusConfigVersion.cmake"
    cmake/xgenStatusCompatibility.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xgen_status)

if(XGS_BUILD_TESTS)
    if(CMAKE_CROSSCOMPILING)
        message(FATAL_ERROR "Build host tests separately from MCU targets")
    endif()
    enable_language(CXX)
    find_package(GTest 1.16.0 EXACT CONFIG REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif()
