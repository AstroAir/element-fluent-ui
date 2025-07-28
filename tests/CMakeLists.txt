# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

project(FluentQtTests)

# Find required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)

# Enable Qt's automatic MOC, UIC, and RCC processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Test configuration
enable_testing()

# Helper function to create tests
function(add_fluent_test test_name source_file)
    add_executable(${test_name} ${source_file})
    
    target_link_libraries(${test_name}
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
        FluentQt  # Link against the main library
    )
    
    # Add the test to CTest
    add_test(NAME ${test_name} COMMAND ${test_name})
    
    # Set test properties
    set_tests_properties(${test_name} PROPERTIES
        TIMEOUT 30
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"  # Run tests headless
    )
endfunction()

# Animation System Tests
add_fluent_test(FluentAnimatorTest FluentAnimatorTest.cpp)

# Theme System Tests
add_fluent_test(FluentThemeTest FluentThemeTest.cpp)

# Form Components Tests
add_fluent_test(FluentFormComponentsTest FluentFormComponentsTest.cpp)

# Feedback Components Tests
add_fluent_test(FluentFeedbackComponentsTest FluentFeedbackComponentsTest.cpp)

# Layout Components Tests
add_fluent_test(FluentLayoutComponentsTest FluentLayoutComponentsTest.cpp)

# Integration Tests
add_fluent_test(FluentIntegrationTest FluentIntegrationTest.cpp)

# Performance Tests
add_fluent_test(FluentPerformanceTest FluentPerformanceTest.cpp)

# Accessibility Tests
add_fluent_test(FluentAccessibilityTest FluentAccessibilityTest.cpp)

# Carousel Component Tests
add_fluent_test(FluentCarouselTest FluentCarouselTest.cpp)
add_fluent_test(FluentCarouselVariantsTest FluentCarouselVariantsTest.cpp)
add_fluent_test(FluentCarouselStylingTest FluentCarouselStylingTest.cpp)

# Custom test target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --parallel 4
    DEPENDS
        FluentAnimatorTest
        FluentThemeTest
        FluentFormComponentsTest
        FluentFeedbackComponentsTest
        FluentLayoutComponentsTest
        FluentIntegrationTest
        FluentPerformanceTest
        FluentAccessibilityTest
        FluentCarouselTest
        FluentCarouselVariantsTest
        FluentCarouselStylingTest
    COMMENT "Running all FluentQt tests"
)

# Test coverage target (if gcov/lcov is available)
find_program(GCOV_PATH gcov)
find_program(LCOV_PATH lcov)
find_program(GENHTML_PATH genhtml)

if(GCOV_PATH AND LCOV_PATH AND GENHTML_PATH)
    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E make_directory coverage
        COMMAND ${LCOV_PATH} --directory . --zerocounters
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage/coverage.info
        COMMAND ${LCOV_PATH} --remove coverage/coverage.info '/usr/*' '*/tests/*' '*/build/*' --output-file coverage/coverage_filtered.info
        COMMAND ${GENHTML_PATH} coverage/coverage_filtered.info --output-directory coverage/html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating test coverage report"
    )
endif()

# Benchmark target for performance tests
add_custom_target(benchmark
    COMMAND FluentPerformanceTest --benchmark
    DEPENDS FluentPerformanceTest
    COMMENT "Running performance benchmarks"
)

# Memory leak detection with Valgrind (Linux only)
find_program(VALGRIND_PATH valgrind)
if(VALGRIND_PATH AND UNIX AND NOT APPLE)
    add_custom_target(memcheck
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -T memcheck
        COMMENT "Running tests with Valgrind memory checking"
    )
endif()

# Test data files (if needed)
file(GLOB TEST_DATA_FILES "data/*")
if(TEST_DATA_FILES)
    file(COPY ${TEST_DATA_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
endif()

# Test configuration file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/test_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_config.h
    @ONLY
)

# Include the generated config
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Compiler-specific test flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(FluentAnimatorTest PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(FluentThemeTest PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(FluentFormComponentsTest PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Test output formatting
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

# Print test summary
message(STATUS "FluentQt Tests Configuration:")
message(STATUS "  - Animation System Tests: FluentAnimatorTest")
message(STATUS "  - Theme System Tests: FluentThemeTest")
message(STATUS "  - Form Components Tests: FluentFormComponentsTest")
message(STATUS "  - Feedback Components Tests: FluentFeedbackComponentsTest")
message(STATUS "  - Layout Components Tests: FluentLayoutComponentsTest")
message(STATUS "  - Integration Tests: FluentIntegrationTest")
message(STATUS "  - Performance Tests: FluentPerformanceTest")
message(STATUS "  - Accessibility Tests: FluentAccessibilityTest")
message(STATUS "  - Carousel Component Tests: FluentCarouselTest")
message(STATUS "  - Carousel Variants Tests: FluentCarouselVariantsTest")
message(STATUS "  - Carousel Styling Tests: FluentCarouselStylingTest")
message(STATUS "")
message(STATUS "Available test targets:")
message(STATUS "  - make run_tests: Run all tests")
if(GCOV_PATH AND LCOV_PATH AND GENHTML_PATH)
    message(STATUS "  - make coverage: Generate coverage report")
endif()
message(STATUS "  - make benchmark: Run performance benchmarks")
if(VALGRIND_PATH AND UNIX AND NOT APPLE)
    message(STATUS "  - make memcheck: Run tests with Valgrind")
endif()
message(STATUS "")
message(STATUS "To run individual tests:")
message(STATUS "  - ctest -R FluentAnimatorTest")
message(STATUS "  - ctest -R FluentThemeTest")
message(STATUS "  - etc.")
