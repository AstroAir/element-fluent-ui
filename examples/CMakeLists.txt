# CMakeLists.txt for FluentQt Examples
cmake_minimum_required(VERSION 3.20)

project(FluentQtExamples VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include FluentQt headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Link to FluentQt library
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Unified Component Showcase - Main comprehensive example
add_executable(UnifiedComponentShowcase
    UnifiedComponentShowcase.cpp
)

target_link_libraries(UnifiedComponentShowcase
    Qt6::Core
    Qt6::Widgets
    FluentQt
)

# Set target properties
set_target_properties(UnifiedComponentShowcase PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Additional examples
add_executable(BasicExample
    main.cpp
)

target_link_libraries(BasicExample
    Qt6::Core
    Qt6::Widgets
    FluentQt
)

add_executable(AdvancedDemo
    advanced_demo.cpp
)

target_link_libraries(AdvancedDemo
    Qt6::Core
    Qt6::Widgets
    FluentQt
)

add_executable(ComprehensiveDemo
    ComprehensiveDemo.cpp
)

target_link_libraries(ComprehensiveDemo
    Qt6::Core
    Qt6::Widgets
    FluentQt
)

# Component-specific examples
add_executable(CarouselShowcase
    CarouselShowcaseExample.cpp
)

target_link_libraries(CarouselShowcase
    Qt6::Core
    Qt6::Widgets
    FluentQt
)

add_executable(FormComponents
    FormComponentsExample.cpp
)

target_link_libraries(FormComponents
    Qt6::Core
    Qt6::Widgets
    FluentQt
)

add_executable(FeedbackComponents
    FeedbackComponentsExample.cpp
)

target_link_libraries(FeedbackComponents
    Qt6::Core
    Qt6::Widgets
    FluentQt
)

# Enhanced FluentAutoCarousel Example
add_executable(EnhancedFluentAutoCarouselExample
    EnhancedFluentAutoCarouselExample.cpp
)

target_link_libraries(EnhancedFluentAutoCarouselExample
    Qt6::Core
    Qt6::Widgets
    FluentQt
)

# Toast and Select Demo
add_subdirectory(ToastAndSelectDemo)

# Installation
install(TARGETS UnifiedComponentShowcase BasicExample AdvancedDemo ComprehensiveDemo
    CarouselShowcase FormComponents FeedbackComponents EnhancedFluentAutoCarouselExample
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

# Copy resources if they exist
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
    install(DIRECTORY resources/ DESTINATION bin/resources)
endif()

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    set_target_properties(UnifiedComponentShowcase PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
elseif(APPLE)
    # macOS-specific settings
    set_target_properties(UnifiedComponentShowcase PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
    )
endif()

# Debug configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(UnifiedComponentShowcase PRIVATE DEBUG_BUILD)
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(UnifiedComponentShowcase PRIVATE /W4)
else()
    target_compile_options(UnifiedComponentShowcase PRIVATE -Wall -Wextra -Wpedantic)
endif()
