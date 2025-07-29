# tests/Accessibility/CMakeLists.txt

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)

# Create the test executable
add_executable(FluentAccessibleTest
    FluentAccessibleTest.cpp
)

# Link against required libraries
target_link_libraries(FluentAccessibleTest
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    FluentQt::Accessibility
)

# Include directories
target_include_directories(FluentAccessibleTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Add the test to CTest
add_test(NAME FluentAccessibleTest COMMAND FluentAccessibleTest)

# Set test properties
set_tests_properties(FluentAccessibleTest PROPERTIES
    TIMEOUT 30
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
