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

# Test options
option(FLUENT_QT_ENABLE_COVERAGE "Enable code coverage" OFF)
option(FLUENT_QT_ENABLE_BENCHMARKS "Enable performance benchmarks" OFF)
option(FLUENT_QT_ENABLE_ACCESSIBILITY "Enable accessibility tests" ON)
option(FLUENT_QT_PARALLEL_TESTS "Run tests in parallel" ON)

# Set test timeout
set(FLUENT_QT_TEST_TIMEOUT 30 CACHE STRING "Test timeout in seconds")

# Configure test environment
if(WIN32)
    set(FLUENT_QT_TEST_PLATFORM "offscreen")
elseif(APPLE)
    set(FLUENT_QT_TEST_PLATFORM "cocoa")
else()
    set(FLUENT_QT_TEST_PLATFORM "offscreen")
endif()

# Helper function to create tests
function(add_fluent_test test_name source_file)
    add_executable(${test_name} ${source_file})

    target_link_libraries(${test_name}
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
        FluentQt  # Link against the main library
    )

    # Include test configuration
    target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

    # Add compiler flags for coverage if enabled
    if(FLUENT_QT_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${test_name} PRIVATE --coverage)
        target_link_options(${test_name} PRIVATE --coverage)
    endif()

    # Add the test to CTest
    add_test(NAME ${test_name} COMMAND ${test_name})

    # Set test properties
    set_tests_properties(${test_name} PROPERTIES
        TIMEOUT ${FLUENT_QT_TEST_TIMEOUT}
        ENVIRONMENT "QT_QPA_PLATFORM=${FLUENT_QT_TEST_PLATFORM}"
    )

    # Add test labels for filtering
    if(${source_file} MATCHES "Integration")
        set_tests_properties(${test_name} PROPERTIES LABELS "integration")
    elseif(${source_file} MATCHES "Accessibility")
        set_tests_properties(${test_name} PROPERTIES LABELS "accessibility")
    elseif(${source_file} MATCHES "Performance")
        set_tests_properties(${test_name} PROPERTIES LABELS "performance")
    else()
        set_tests_properties(${test_name} PROPERTIES LABELS "unit")
    endif()
endfunction()

# Animation System Tests
add_fluent_test(FluentAnimatorTest FluentAnimatorTest.cpp)
add_fluent_test(FluentAnimatorTestFixed FluentAnimatorTestFixed.cpp)

# Theme System Tests
add_fluent_test(FluentThemeTest FluentThemeTest.cpp)

# Form Components Tests
add_fluent_test(FluentFormComponentsTest FluentFormComponentsTest.cpp)

# Button Components Tests
add_fluent_test(FluentButtonTest Components/FluentButtonTest.cpp)

# Radio Button Components Tests
add_fluent_test(FluentRadioButtonTest Components/FluentRadioButtonTest.cpp)

# Spin Box Components Tests
add_fluent_test(FluentSpinBoxTest Components/FluentSpinBoxTest.cpp)

# Layout Components Tests
add_fluent_test(FluentCardTest Components/FluentCardTest.cpp)

# Feedback Components Tests
add_fluent_test(FluentProgressBarTest Components/FluentProgressBarTest.cpp)
add_fluent_test(FluentTooltipTest Components/FluentTooltipTest.cpp)
add_fluent_test(FluentBadgeTest Components/FluentBadgeTest.cpp)

# Specialized Components Tests
add_fluent_test(FluentCalendarTest Components/FluentCalendarTest.cpp)

# Core System Tests
add_fluent_test(FluentComponentTest Core/FluentComponentTest.cpp)

# Integration Tests
add_fluent_test(FluentIntegrationTest Integration/FluentIntegrationTest.cpp)

# Accessibility Tests
add_fluent_test(FluentAccessibilityTest Accessibility/FluentAccessibilityTest.cpp)

# Legacy test files (if they exist)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/FluentFeedbackComponentsTest.cpp)
    add_fluent_test(FluentFeedbackComponentsTest FluentFeedbackComponentsTest.cpp)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/FluentLayoutComponentsTest.cpp)
    add_fluent_test(FluentLayoutComponentsTest FluentLayoutComponentsTest.cpp)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/FluentPerformanceTest.cpp)
    add_fluent_test(FluentPerformanceTest FluentPerformanceTest.cpp)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/FluentCarouselTest.cpp)
    add_fluent_test(FluentCarouselTest FluentCarouselTest.cpp)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/FluentCarouselVariantsTest.cpp)
    add_fluent_test(FluentCarouselVariantsTest FluentCarouselVariantsTest.cpp)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/FluentCarouselStylingTest.cpp)
    add_fluent_test(FluentCarouselStylingTest FluentCarouselStylingTest.cpp)
endif()
add_fluent_test(FluentCarouselCoalesceTest FluentCarouselCoalesceTest.cpp)

# TreeView Performance/Behavior Tests
add_fluent_test(FluentTreeViewPerfTest Components/FluentTreeViewPerfTest.cpp)


# Custom test target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --parallel 4
    DEPENDS
        # Core System Tests
        FluentAnimatorTest
        FluentThemeTest
        FluentComponentTest

        # Form Components Tests
        FluentFormComponentsTest

        # Button Components Tests
        FluentButtonTest
        FluentRadioButtonTest
        FluentSpinBoxTest

        # Layout Components Tests
        FluentCardTest

        # Feedback Components Tests
        FluentProgressBarTest
        FluentTooltipTest
        FluentBadgeTest

        # Specialized Components Tests
        FluentCalendarTest

        # Integration and Accessibility Tests
        FluentIntegrationTest
        FluentAccessibilityTest
    COMMENT "Running all FluentQt tests"
)

# Test coverage target (if gcov/lcov is available)
if(FLUENT_QT_ENABLE_COVERAGE)
    find_program(GCOV_PATH gcov)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)

    if(GCOV_PATH AND LCOV_PATH AND GENHTML_PATH)
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory coverage
            COMMAND ${LCOV_PATH} --directory . --zerocounters
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --parallel ${CMAKE_BUILD_PARALLEL_LEVEL}
            COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage/coverage.info
            COMMAND ${LCOV_PATH} --remove coverage/coverage.info
                '/usr/*'
                '*/tests/*'
                '*/build/*'
                '*/Qt6/*'
                '*/qt6/*'
                --output-file coverage/coverage_filtered.info
            COMMAND ${GENHTML_PATH} coverage/coverage_filtered.info
                --output-directory coverage/html
                --title "FluentQt Test Coverage"
                --show-details
                --legend
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating test coverage report"
        )

        # Add coverage info to the summary
        set(FLUENT_QT_COVERAGE_ENABLED TRUE)
    else()
        message(WARNING "Coverage tools not found. Install gcov, lcov, and genhtml to enable coverage reporting.")
        set(FLUENT_QT_COVERAGE_ENABLED FALSE)
    endif()
else()
    set(FLUENT_QT_COVERAGE_ENABLED FALSE)
endif()

# Test category targets
add_custom_target(test_unit
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L unit
    COMMENT "Running unit tests"
)

add_custom_target(test_integration
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L integration
    COMMENT "Running integration tests"
)

add_custom_target(test_accessibility
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L accessibility
    COMMENT "Running accessibility tests"
)

add_custom_target(test_performance
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L performance
    COMMENT "Running performance tests"
)

# Benchmark target for performance tests
if(FLUENT_QT_ENABLE_BENCHMARKS)
    add_custom_target(benchmark
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L performance --verbose
        COMMENT "Running performance benchmarks"
    )
endif()

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
set(FLUENT_QT_TEST_COVERAGE_ENABLED ${FLUENT_QT_COVERAGE_ENABLED})
set(FLUENT_QT_TEST_VALGRIND_ENABLED ${VALGRIND_PATH})
set(FLUENT_QT_TEST_BENCHMARK_ENABLED ${FLUENT_QT_ENABLE_BENCHMARKS})

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
message(STATUS "")
message(STATUS "FluentQt Tests Configuration:")
message(STATUS "=============================")
message(STATUS "Test Options:")
message(STATUS "  - Coverage enabled: ${FLUENT_QT_ENABLE_COVERAGE}")
message(STATUS "  - Benchmarks enabled: ${FLUENT_QT_ENABLE_BENCHMARKS}")
message(STATUS "  - Accessibility enabled: ${FLUENT_QT_ENABLE_ACCESSIBILITY}")
message(STATUS "  - Parallel tests: ${FLUENT_QT_PARALLEL_TESTS}")
message(STATUS "  - Test timeout: ${FLUENT_QT_TEST_TIMEOUT}s")
message(STATUS "  - Test platform: ${FLUENT_QT_TEST_PLATFORM}")
message(STATUS "")
message(STATUS "Test Categories:")
message(STATUS "  - Core System Tests: FluentAnimatorTest, FluentThemeTest, FluentComponentTest")
message(STATUS "  - Form Components: FluentFormComponentsTest")
message(STATUS "  - Button Components: FluentButtonTest, FluentRadioButtonTest, FluentSpinBoxTest")
message(STATUS "  - Layout Components: FluentCardTest")
message(STATUS "  - Feedback Components: FluentProgressBarTest, FluentTooltipTest, FluentBadgeTest")
message(STATUS "  - Specialized Components: FluentCalendarTest")
message(STATUS "  - Integration Tests: FluentIntegrationTest")
message(STATUS "  - Accessibility Tests: FluentAccessibilityTest")
message(STATUS "")
message(STATUS "Available test targets:")
message(STATUS "  - make run_tests: Run all tests")
message(STATUS "  - make test_unit: Run unit tests only")
message(STATUS "  - make test_integration: Run integration tests only")
message(STATUS "  - make test_accessibility: Run accessibility tests only")
message(STATUS "  - make test_performance: Run performance tests only")
if(FLUENT_QT_COVERAGE_ENABLED)
    message(STATUS "  - make coverage: Generate coverage report")
endif()
if(FLUENT_QT_ENABLE_BENCHMARKS)
    message(STATUS "  - make benchmark: Run performance benchmarks")
endif()
if(VALGRIND_PATH AND UNIX AND NOT APPLE)
    message(STATUS "  - make memcheck: Run tests with Valgrind")
endif()
message(STATUS "")
message(STATUS "Test Runner:")
message(STATUS "  - Python runner: python3 tests/run_tests.py")
message(STATUS "  - Run specific test: python3 tests/run_tests.py --test FluentButtonTest")
message(STATUS "  - Generate coverage: python3 tests/run_tests.py --coverage")
message(STATUS "  - Memory check: python3 tests/run_tests.py --memcheck FluentButtonTest")
message(STATUS "")
message(STATUS "CTest Commands:")
message(STATUS "  - Run all tests: ctest --output-on-failure")
message(STATUS "  - Run by label: ctest -L unit")
message(STATUS "  - Run specific test: ctest -R FluentButtonTest")
message(STATUS "  - Parallel execution: ctest -j4")
message(STATUS "")
