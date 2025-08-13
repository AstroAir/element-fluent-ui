# CMakeLists.txt - Element Fluent UI Library
cmake_minimum_required(VERSION 3.20)

# Project definition with metadata
project(FluentQt
    VERSION 1.0.0
    DESCRIPTION "A modern, comprehensive C++ Qt6 library implementing Microsoft's Fluent Design System"
    HOMEPAGE_URL "https://github.com/ElementAstro/element-fluent-ui"
    LANGUAGES CXX
)

# ============================================================================
# Project Configuration
# ============================================================================

# Include FluentQt platform detection and toolchain configuration
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(FluentQtPlatformDetection)
include(FluentQtToolchains)

# Detect platform and configure toolchain
fluentqt_detect_platform()
fluentqt_validate_toolchain()
fluentqt_configure_toolchain()

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type default
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Options
option(FLUENTQT_BUILD_EXAMPLES "Build example applications" ON)
option(FLUENTQT_BUILD_TESTS "Build test suite" ON)
option(FLUENTQT_BUILD_DOCS "Build documentation" OFF)
option(FLUENTQT_INSTALL "Generate install target" ON)
option(FLUENTQT_BUILD_SHARED "Build shared library" ON)
option(FLUENTQT_ENABLE_CHARTS "Enable chart components (requires Qt6Charts)" ON)
option(FLUENTQT_ENABLE_MULTIMEDIA "Enable multimedia components (requires Qt6Multimedia)" OFF)
option(FLUENTQT_ENABLE_NETWORK "Enable network components (requires Qt6Network)" OFF)
option(FLUENTQT_ENABLE_LTO "Enable Link Time Optimization" OFF)
option(FLUENTQT_ENABLE_SANITIZERS "Enable sanitizers for debugging" OFF)

# ============================================================================
# Dependencies
# ============================================================================

# Find Qt6 core components
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Gui
    PrintSupport
)

# Optional Qt6 components
if(FLUENTQT_ENABLE_CHARTS)
    find_package(Qt6 QUIET COMPONENTS Charts)
    if(Qt6Charts_FOUND)
        set(FLUENTQT_CHARTS_AVAILABLE TRUE)
        message(STATUS "Qt6 Charts found - FluentChartView will be available")
    else()
        set(FLUENTQT_CHARTS_AVAILABLE FALSE)
        message(WARNING "Qt6 Charts not found - FluentChartView will be disabled")
    endif()
endif()

if(FLUENTQT_ENABLE_MULTIMEDIA)
    find_package(Qt6 QUIET COMPONENTS Multimedia)
    if(Qt6Multimedia_FOUND)
        set(FLUENTQT_MULTIMEDIA_AVAILABLE TRUE)
        message(STATUS "Qt6 Multimedia found - Multimedia components will be available")
    else()
        message(WARNING "Qt6 Multimedia not found - Multimedia components will be disabled")
    endif()
endif()

if(FLUENTQT_ENABLE_NETWORK)
    find_package(Qt6 QUIET COMPONENTS Network)
    if(Qt6Network_FOUND)
        set(FLUENTQT_NETWORK_AVAILABLE TRUE)
        message(STATUS "Qt6 Network found - Network components will be available")
    else()
        message(WARNING "Qt6 Network not found - Network components will be disabled")
    endif()
endif()

# Testing framework
if(FLUENTQT_BUILD_TESTS)
    find_package(Qt6 QUIET COMPONENTS Test)
    if(NOT Qt6Test_FOUND)
        message(WARNING "Qt6 Test not found - tests will be disabled")
        set(FLUENTQT_BUILD_TESTS OFF)
    endif()
endif()

# Validate all dependencies
fluentqt_validate_dependencies()

# ============================================================================
# Qt Configuration
# ============================================================================

# Enable Qt MOC, RCC, and UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Ensure MOC finds all headers
set(CMAKE_AUTOMOC_MOC_OPTIONS "-I${CMAKE_CURRENT_SOURCE_DIR}/include")
set_property(DIRECTORY PROPERTY CMAKE_AUTOMOC_RELAXED_MODE ON)

# Qt6 requires position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ============================================================================
# Compiler Configuration
# ============================================================================

# Compiler-specific settings
if(MSVC)
    # MSVC specific options
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2 /Ob2 /DNDEBUG)
    endif()
    # Enable parallel compilation
    add_compile_options(/MP)
else()
    # GCC/Clang options
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    endif()
endif()

# Link Time Optimization
if(FLUENTQT_ENABLE_LTO AND NOT FLUENTQT_BUILD_TESTS)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT lto_error)
    if(lto_supported)
        message(STATUS "Link Time Optimization enabled")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "LTO not supported: ${lto_error}")
    endif()
else()
    # Explicitly disable LTO flags that might leak in from environment/toolchain defaults
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fno-lto)
        add_link_options(-fno-lto)
    endif()
    if(FLUENTQT_BUILD_TESTS)
        message(STATUS "Link Time Optimization disabled when building tests to avoid MinGW pseudo-relocation issues")
    endif()
endif()

# Sanitizers for debugging
if(FLUENTQT_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(NOT MSVC AND NOT MINGW)
        add_compile_options(-fsanitize=address -fsanitize=undefined)
        add_link_options(-fsanitize=address -fsanitize=undefined)
        message(STATUS "Sanitizers enabled")
    else()
        message(STATUS "Sanitizers disabled (not supported on MSVC/MinGW)")
    endif()
endif()

# ============================================================================
# Source Files Organization
# ============================================================================

# Core system sources
set(FLUENTQT_CORE_SOURCES
    src/Core/FluentComponent.cpp
    src/Core/FluentPerformance.cpp
    src/Core/FluentLazyComponent.cpp
    src/Core/FluentBundleAnalyzer.cpp
    src/Core/FluentFluidTypography.cpp
    src/Core/FluentResponsive.cpp
    src/Core/FluentResponsiveComponent.cpp
    src/Core/FluentResponsiveLayout.cpp
    src/Core/FluentCulturalAdaptation.cpp
    src/Core/FluentRTLSupport.cpp
    src/Core/FluentLocaleFormatting.cpp
)

# Styling system sources
set(FLUENTQT_STYLING_SOURCES
    src/Styling/FluentTheme.cpp
    src/Styling/FluentAdvancedThemeManager.cpp
    src/Styling/FluentCarouselStyles.cpp
)

# Animation system sources
set(FLUENTQT_ANIMATION_SOURCES
    src/Animation/FluentAnimator.cpp
    src/Animation/FluentAnimationPerformanceManager.cpp
    src/Animation/FluentTransformEffect.cpp
)

# Component sources
set(FLUENTQT_COMPONENT_SOURCES
    src/Components/FluentButton.cpp
    src/Components/FluentCard.cpp
    src/Components/FluentComboBox.cpp
    src/Components/FluentCalendar.cpp
    src/Components/FluentNavigationView.cpp
    src/Components/FluentRichTextEditor.cpp
    src/Components/FluentTabView.cpp
    src/Components/FluentTreeView.cpp
    src/Components/FluentContextMenu.cpp
    src/Components/FluentSplitter.cpp
    src/Components/FluentCarousel.cpp
    src/Components/FluentBasicCarousel.cpp
    src/Components/FluentAutoCarousel.cpp
    src/Components/FluentIndicatorCarousel.cpp
    src/Components/FluentTouchCarousel.cpp
    src/Components/FluentTimeline.cpp
    src/Components/FluentTimelineItem.cpp
    src/Components/FluentToast.cpp
    src/Components/FluentToastManager.cpp
    src/Components/FluentTooltip.cpp
    src/Components/FluentSelect.cpp
    src/Components/FluentSelectItem.cpp
    src/Components/FluentSelectDropdown.cpp
    src/Components/FluentResizable.cpp
    src/Components/FluentScrollArea.cpp
    src/Components/FluentSheet.cpp
    # Form and input components
    src/Components/FluentBadge.cpp
    src/Components/FluentCheckBox.cpp
    src/Components/FluentTextInput.cpp
    src/Components/FluentSlider.cpp
    # Temporarily disabled - requires major architectural changes:
    # src/Components/FluentRadioButton.cpp
    # Component implementations that are currently complete
    src/Components/FluentProgressBar.cpp
)

# Accessibility sources
set(FLUENTQT_ACCESSIBILITY_SOURCES
    src/Accessibility/FluentAccessible.cpp
    src/Accessibility/FluentAccessibilityManager.cpp
    src/Accessibility/FluentAccessibilityManagerEnhanced.cpp
    src/Accessibility/FluentScreenReaderManager.cpp
    src/Accessibility/FluentKeyboardNavigationManager.cpp
    src/Accessibility/FluentWcagChecker.cpp
)

# Combine all sources
set(FLUENTQT_SOURCES
    ${FLUENTQT_CORE_SOURCES}
    ${FLUENTQT_STYLING_SOURCES}
    ${FLUENTQT_ANIMATION_SOURCES}
    ${FLUENTQT_COMPONENT_SOURCES}
    ${FLUENTQT_ACCESSIBILITY_SOURCES}
)

# Core system headers
set(FLUENTQT_CORE_HEADERS
    include/FluentQt/Core/FluentComponent.h
    include/FluentQt/Core/FluentState.h
    include/FluentQt/Core/FluentPerformance.h
    include/FluentQt/Core/FluentLazyComponent.h
    include/FluentQt/Core/FluentBundleAnalyzer.h
    include/FluentQt/Core/FluentFluidTypography.h
    include/FluentQt/Core/FluentResponsive.h
    include/FluentQt/Core/FluentResponsiveComponent.h
    include/FluentQt/Core/FluentResponsiveLayout.h
    include/FluentQt/Core/FluentCulturalAdaptation.h
    include/FluentQt/Core/FluentRTLSupport.h
    include/FluentQt/Core/FluentLocaleFormatting.h
)

# Styling system headers
set(FLUENTQT_STYLING_HEADERS
    include/FluentQt/Styling/FluentTheme.h
    include/FluentQt/Styling/FluentAdvancedThemeManager.h
    include/FluentQt/Styling/FluentCarouselStyles.h
)

# Animation system headers
set(FLUENTQT_ANIMATION_HEADERS
    include/FluentQt/Animation/FluentAnimator.h
    include/FluentQt/Animation/FluentAnimationPerformanceManager.h
    include/FluentQt/Animation/FluentTransformEffect.h
)

# Component headers
set(FLUENTQT_COMPONENT_HEADERS
    include/FluentQt/Components/FluentButton.h
    include/FluentQt/Components/FluentCard.h
    include/FluentQt/Components/FluentComboBox.h
    include/FluentQt/Components/FluentCalendar.h
    include/FluentQt/Components/FluentNavigationView.h
    include/FluentQt/Components/FluentRichTextEditor.h
    include/FluentQt/Components/FluentTabView.h
    include/FluentQt/Components/FluentTreeView.h
    include/FluentQt/Components/FluentContextMenu.h
    include/FluentQt/Components/FluentSplitter.h
    include/FluentQt/Components/FluentCarousel.h
    include/FluentQt/Components/FluentBasicCarousel.h
    include/FluentQt/Components/FluentAutoCarousel.h
    include/FluentQt/Components/FluentIndicatorCarousel.h
    include/FluentQt/Components/FluentTouchCarousel.h
    include/FluentQt/Components/FluentTimeline.h
    include/FluentQt/Components/FluentTimelineItem.h
    include/FluentQt/Components/FluentToast.h
    include/FluentQt/Components/FluentToastManager.h
    include/FluentQt/Components/FluentTooltip.h
    include/FluentQt/Components/FluentSelect.h
    include/FluentQt/Components/FluentSelectItem.h
    include/FluentQt/Components/FluentSelectDropdown.h
    include/FluentQt/Components/FluentResizable.h
    include/FluentQt/Components/FluentScrollArea.h
    include/FluentQt/Components/FluentSheet.h
    # Form and input component headers
    include/FluentQt/Components/FluentBadge.h
    include/FluentQt/Components/FluentCheckBox.h
    include/FluentQt/Components/FluentTextInput.h
    include/FluentQt/Components/FluentSlider.h
    # Temporarily disabled - requires major architectural changes:
    # include/FluentQt/Components/FluentRadioButton.h
    include/FluentQt/Components/FluentProgressBar.h
)

# Accessibility headers
set(FLUENTQT_ACCESSIBILITY_HEADERS
    include/FluentQt/Accessibility/FluentAccessible.h
    include/FluentQt/Accessibility/FluentAccessibilityManager.h
    include/FluentQt/Accessibility/FluentScreenReaderManager.h
    include/FluentQt/Accessibility/FluentKeyboardNavigationManager.h
    include/FluentQt/Accessibility/FluentWcagChecker.h
)

# Main library headers
set(FLUENTQT_MAIN_HEADERS
    include/FluentQt/FluentQt.h
    include/FluentQt/Components.h
    include/FluentQt/Styling.h
    include/FluentQt/Animation.h
    include/FluentQt/Accessibility.h
    include/FluentQt/index.h
)

# Combine all headers
set(FLUENTQT_HEADERS
    ${FLUENTQT_CORE_HEADERS}
    ${FLUENTQT_STYLING_HEADERS}
    ${FLUENTQT_ANIMATION_HEADERS}
    ${FLUENTQT_COMPONENT_HEADERS}
    ${FLUENTQT_ACCESSIBILITY_HEADERS}
    ${FLUENTQT_MAIN_HEADERS}
)

# Conditionally add optional components
if(FLUENTQT_CHARTS_AVAILABLE)
    list(APPEND FLUENTQT_SOURCES src/Components/FluentChartView.cpp)
    list(APPEND FLUENTQT_HEADERS include/FluentQt/Components/FluentChartView.h)
endif()

if(FLUENTQT_MULTIMEDIA_AVAILABLE)
    # Add multimedia components when available
    # list(APPEND FLUENTQT_SOURCES src/Components/FluentMediaPlayer.cpp)
    # list(APPEND FLUENTQT_HEADERS include/FluentQt/Components/FluentMediaPlayer.h)
endif()

if(FLUENTQT_NETWORK_AVAILABLE)
    # Add network components when available
    # list(APPEND FLUENTQT_SOURCES src/Components/FluentNetworkManager.cpp)
    # list(APPEND FLUENTQT_HEADERS include/FluentQt/Components/FluentNetworkManager.h)
endif()

# ============================================================================
# Library Creation
# ============================================================================

# Create the main library
if(FLUENTQT_BUILD_SHARED)
    add_library(FluentQt SHARED ${FLUENTQT_SOURCES} ${FLUENTQT_HEADERS})
    set_target_properties(FluentQt PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
else()
    add_library(FluentQt STATIC ${FLUENTQT_SOURCES} ${FLUENTQT_HEADERS})
endif()

# Create alias for consistent usage
add_library(FluentQt::FluentQt ALIAS FluentQt)

# ============================================================================
# Target Configuration
# ============================================================================

# Include directories
target_include_directories(FluentQt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(FluentQt
    PUBLIC
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        Qt6::PrintSupport
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(FluentQt PRIVATE dwmapi user32 gdi32)
    target_compile_definitions(FluentQt PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    if(COCOA_LIBRARY)
        target_link_libraries(FluentQt PRIVATE ${COCOA_LIBRARY})
    endif()
elseif(UNIX)
    # Linux-specific libraries if needed
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(GTK3 QUIET gtk+-3.0)
        if(GTK3_FOUND)
            target_compile_definitions(FluentQt PRIVATE FLUENTQT_GTK3_AVAILABLE)
        endif()
    endif()
endif()

# Conditionally link optional Qt components
if(FLUENTQT_CHARTS_AVAILABLE)
    target_link_libraries(FluentQt PUBLIC Qt6::Charts)
    target_compile_definitions(FluentQt PUBLIC FLUENTQT_CHARTS_AVAILABLE)
endif()

if(FLUENTQT_MULTIMEDIA_AVAILABLE)
    target_link_libraries(FluentQt PUBLIC Qt6::Multimedia)
    target_compile_definitions(FluentQt PUBLIC FLUENTQT_MULTIMEDIA_AVAILABLE)
endif()

if(FLUENTQT_NETWORK_AVAILABLE)
    target_link_libraries(FluentQt PUBLIC Qt6::Network)
    target_compile_definitions(FluentQt PUBLIC FLUENTQT_NETWORK_AVAILABLE)
endif()

# ============================================================================
# Compile Definitions
# ============================================================================

# Version information
target_compile_definitions(FluentQt PUBLIC
    FLUENTQT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    FLUENTQT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    FLUENTQT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    FLUENTQT_VERSION_STRING="${PROJECT_VERSION}"
)

# Build configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(FluentQt PUBLIC FLUENTQT_DEBUG)
else()
    target_compile_definitions(FluentQt PUBLIC FLUENTQT_RELEASE)
endif()

# Feature flags
if(FLUENTQT_BUILD_SHARED)
    target_compile_definitions(FluentQt PUBLIC FLUENTQT_SHARED)
    if(WIN32)
        target_compile_definitions(FluentQt PRIVATE FLUENTQT_LIBRARY)
    endif()
else()
    target_compile_definitions(FluentQt PUBLIC FLUENTQT_STATIC)
endif()

# Qt version compatibility
target_compile_definitions(FluentQt PUBLIC
    QT_DISABLE_DEPRECATED_BEFORE=0x060000

)

# ============================================================================
# Installation Configuration
# ============================================================================

if(FLUENTQT_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Install the library
    install(TARGETS FluentQt
        EXPORT FluentQtTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    # Install headers
    install(DIRECTORY include/FluentQt
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
    )

    # Install CMake config files
    set(ConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake/FluentQt)

    # Create config file
    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FluentQtConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/FluentQtConfig.cmake"
        INSTALL_DESTINATION ${ConfigPackageLocation}
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
    )

    # Create version file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/FluentQtConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    # Install config files
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/FluentQtConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/FluentQtConfigVersion.cmake"
        DESTINATION ${ConfigPackageLocation}
    )

    # Install targets file
    install(EXPORT FluentQtTargets
        FILE FluentQtTargets.cmake
        NAMESPACE FluentQt::
        DESTINATION ${ConfigPackageLocation}
    )

    # Create pkg-config file
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FluentQt.pc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/FluentQt.pc"
        @ONLY
    )

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/FluentQt.pc"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()

# ============================================================================
# Examples
# ============================================================================

if(FLUENTQT_BUILD_EXAMPLES)
    # Main demo application
    add_executable(FluentQtDemo examples/main.cpp)
    target_link_libraries(FluentQtDemo FluentQt::FluentQt)
    set_target_properties(FluentQtDemo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    )

    # Carousel showcase example
    add_executable(CarouselShowcase examples/CarouselShowcaseExample.cpp)
    target_link_libraries(CarouselShowcase FluentQt::FluentQt)
    set_target_properties(CarouselShowcase PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    )

    # Timeline examples
    add_executable(TimelineShowcase examples/TimelineShowcaseExample.cpp)
    target_link_libraries(TimelineShowcase FluentQt::FluentQt)
    set_target_properties(TimelineShowcase PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    )

    add_executable(SimpleTimeline examples/SimpleTimelineExample.cpp)
    target_link_libraries(SimpleTimeline FluentQt::FluentQt)
    set_target_properties(SimpleTimeline PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    )

    # Toast and Select demo
    add_executable(ToastAndSelectDemo examples/ToastAndSelectDemo/main.cpp)
    target_link_libraries(ToastAndSelectDemo FluentQt::FluentQt)
    set_target_properties(ToastAndSelectDemo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    )

    # Install examples if requested
    if(FLUENTQT_INSTALL)
        install(TARGETS
            FluentQtDemo
            CarouselShowcase
            TimelineShowcase
            SimpleTimeline
            ToastAndSelectDemo
            DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
        )
    endif()
endif()

# ============================================================================
# Testing
# ============================================================================

if(FLUENTQT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ============================================================================
# Documentation
# ============================================================================

if(FLUENTQT_BUILD_DOCS)
    find_package(Doxygen QUIET)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(WARNING "Doxygen not found - documentation will not be built")
    endif()
endif()

# ============================================================================
# CPack Configuration
# ============================================================================

if(FLUENTQT_INSTALL)
    set(CPACK_PACKAGE_NAME "FluentQt")
    set(CPACK_PACKAGE_VENDOR "Element Astro")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern C++ Qt6 library implementing Microsoft's Fluent Design System")
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
    set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/ElementAstro/element-fluent-ui")
    set(CPACK_PACKAGE_CONTACT "Max Qian <astro_air@126.com>")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

    # Platform-specific packaging
    if(WIN32)
        set(CPACK_GENERATOR "NSIS;ZIP")
        set(CPACK_NSIS_DISPLAY_NAME "FluentQt ${PROJECT_VERSION}")
        set(CPACK_NSIS_PACKAGE_NAME "FluentQt")
        set(CPACK_NSIS_URL_INFO_ABOUT "${CPACK_PACKAGE_HOMEPAGE_URL}")
    elseif(APPLE)
        set(CPACK_GENERATOR "DragNDrop;TGZ")
    else()
        set(CPACK_GENERATOR "DEB;RPM;TGZ")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6widgets6, libqt6gui6")
        set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase")
    endif()

    include(CPack)
endif()

# ============================================================================
# Build Summary
# ============================================================================

message(STATUS "")
message(STATUS "FluentQt Configuration Summary:")
message(STATUS "  Version:              ${PROJECT_VERSION}")
message(STATUS "  Build type:           ${CMAKE_BUILD_TYPE}")
message(STATUS "  Shared library:       ${FLUENTQT_BUILD_SHARED}")
message(STATUS "  Install target:       ${FLUENTQT_INSTALL}")
message(STATUS "  Build examples:       ${FLUENTQT_BUILD_EXAMPLES}")
message(STATUS "  Build tests:          ${FLUENTQT_BUILD_TESTS}")
message(STATUS "  Build documentation: ${FLUENTQT_BUILD_DOCS}")
message(STATUS "")
message(STATUS "Optional Features:")
message(STATUS "  Qt6 Charts:           ${FLUENTQT_CHARTS_AVAILABLE}")
message(STATUS "  Qt6 Multimedia:       ${FLUENTQT_MULTIMEDIA_AVAILABLE}")
message(STATUS "  Qt6 Network:          ${FLUENTQT_NETWORK_AVAILABLE}")
message(STATUS "  Link Time Optimization: ${FLUENTQT_ENABLE_LTO}")
message(STATUS "  Sanitizers:           ${FLUENTQT_ENABLE_SANITIZERS}")
message(STATUS "")
message(STATUS "Compiler:")
message(STATUS "  C++ Standard:         ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler:             ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Flags:                ${CMAKE_CXX_FLAGS}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "  Debug flags:          ${CMAKE_CXX_FLAGS_DEBUG}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "  Release flags:        ${CMAKE_CXX_FLAGS_RELEASE}")
endif()
message(STATUS "")
message(STATUS "Installation:")
message(STATUS "  Install prefix:       ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Library directory:    ${CMAKE_INSTALL_FULL_LIBDIR}")
message(STATUS "  Include directory:    ${CMAKE_INSTALL_FULL_INCLUDEDIR}")
message(STATUS "  Binary directory:     ${CMAKE_INSTALL_FULL_BINDIR}")
message(STATUS "")
