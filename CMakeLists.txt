cmake_minimum_required(VERSION 3.20)

project(Relacy
    VERSION 0.1.0
    LANGUAGES CXX
)

option(RELACY_BUILD_TESTS "Build Relacy tests" ${PROJECT_IS_TOP_LEVEL})
option(RELACY_BUILD_EXAMPLES "Build Relacy examples" ${PROJECT_IS_TOP_LEVEL})

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(relacy INTERFACE)
add_library(relacy::relacy ALIAS relacy)
target_include_directories(relacy INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(relacy INTERFACE cxx_std_11)

add_library(relacy_fakestd INTERFACE)
target_include_directories(relacy_fakestd INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/relacy/fakestd>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(RELACY_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/relacy")
set(RELACY_PACKAGE_VERSION "${PROJECT_VERSION}")

install(TARGETS relacy
    EXPORT relacyTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/relacy/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/relacy"
)

install(EXPORT relacyTargets
    NAMESPACE relacy::
    FILE relacyTargets.cmake
    DESTINATION "${RELACY_INSTALL_CMAKEDIR}"
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/relacyConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/relacyConfig.cmake"
    INSTALL_DESTINATION "${RELACY_INSTALL_CMAKEDIR}"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/relacyConfigVersion.cmake"
    VERSION "${RELACY_PACKAGE_VERSION}"
    COMPATIBILITY ExactVersion
    ARCH_INDEPENDENT
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/relacyConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/relacyConfigVersion.cmake"
    DESTINATION "${RELACY_INSTALL_CMAKEDIR}"
)

function(add_relacy_test target_name source_file)
    cmake_parse_arguments(ARG "USE_FAKESTD" "" "" ${ARGN})
    
    add_executable(${target_name} ${source_file})
    
    if(ARG_USE_FAKESTD)
        target_link_libraries(${target_name} PRIVATE relacy_fakestd)
    else()
        target_link_libraries(${target_name} PRIVATE relacy)
    endif()
    
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    )
    
    # Add as a test
    add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()

function(add_relacy_example target_name source_dir)
    file(GLOB_RECURSE example_sources "${source_dir}/*.cpp")
    
    add_executable(${target_name} ${example_sources})
    target_link_libraries(${target_name} PRIVATE relacy)
    
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/example/${target_name}"
    )
    
    # Add as a test
    add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()

if(RELACY_BUILD_TESTS)
    enable_testing()
    add_relacy_test(atomic_flag test/atomic_flag.cpp USE_FAKESTD)
    add_relacy_test(atomic test/atomic.cpp USE_FAKESTD)
    add_relacy_test(atomic_wait_notify test/atomic_wait_notify.cpp USE_FAKESTD)
    add_relacy_test(cxx11_thread test/cxx11_thread.cpp USE_FAKESTD)
    add_relacy_test(main_test test/main.cpp USE_FAKESTD)
    add_relacy_test(stdmutex_test test/stdmutex.cpp USE_FAKESTD)
    
    add_relacy_test(debug_info test/debug_info.cpp)
    add_relacy_test(defaulted_debug_info test/defaulted_debug_info.cpp)
    add_relacy_test(new_delete test/new_delete.cpp)
    
    file(GLOB_RECURSE ntest_sources "test/ntest/*.cpp")
    if(ntest_sources)
        add_executable(ntest ${ntest_sources})
        target_link_libraries(ntest PRIVATE relacy)
        set_target_properties(ntest PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/ntest"
        )
        add_test(NAME ntest COMMAND ntest)
    endif()
    
    add_relacy_test(with_std_overrides test/stdlib/with_std_overrides.cpp USE_FAKESTD)
    add_relacy_test(without_std_overrides test/stdlib/without_std_overrides.cpp)
endif()

if(RELACY_BUILD_EXAMPLES)
    add_relacy_example(cli_ws_deque example/cli_ws_deque)
endif()
