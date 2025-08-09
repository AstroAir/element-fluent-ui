# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(FluentQt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Gui
    PrintSupport
)

# Optional Qt6 Charts (for FluentChartView)
find_package(Qt6 QUIET COMPONENTS Charts)
if(Qt6Charts_FOUND)
    set(FLUENT_CHARTS_AVAILABLE TRUE)
    message(STATUS "Qt6 Charts found - FluentChartView will be available")
else()
    set(FLUENT_CHARTS_AVAILABLE FALSE)
    message(STATUS "Qt6 Charts not found - FluentChartView will be disabled")
endif()

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# FluentQt Library
set(FLUENTQT_SOURCES
    src/Core/FluentComponent.cpp
    src/Core/FluentPerformance.cpp
    src/Styling/FluentTheme.cpp
    src/Animation/FluentAnimator.cpp
    src/Animation/FluentTransformEffect.cpp
    src/Animation/FluentAnimation.cpp
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
    src/Components/FluentSelect.cpp
    src/Components/FluentSelectItem.cpp
    src/Components/FluentSelectDropdown.cpp
    src/Components/FluentResizable.cpp
    src/Components/FluentScrollArea.cpp
    src/Components/FluentSheet.cpp
    src/Styling/FluentCarouselStyles.cpp
    src/Accessibility/FluentAccessible.cpp
)

set(FLUENTQT_HEADERS
    include/FluentQt/Core/FluentComponent.h
    include/FluentQt/Core/FluentState.h
    include/FluentQt/Core/FluentPerformance.h
    include/FluentQt/Styling/FluentTheme.h
    include/FluentQt/Animation/FluentAnimator.h
    include/FluentQt/Animation/FluentTransformEffect.h
    include/FluentQt/Animation/FluentAnimation.h
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
    include/FluentQt/Components/FluentSelect.h
    include/FluentQt/Components/FluentSelectItem.h
    include/FluentQt/Components/FluentSelectDropdown.h
    include/FluentQt/Components/FluentResizable.h
    include/FluentQt/Components/FluentScrollArea.h
    include/FluentQt/Components/FluentSheet.h
    include/FluentQt/Styling/FluentCarouselStyles.h
    include/FluentQt/Accessibility/FluentAccessible.h
)

# Conditionally add Charts-dependent components
if(FLUENT_CHARTS_AVAILABLE)
    list(APPEND FLUENTQT_SOURCES src/Components/FluentChartView.cpp)
    list(APPEND FLUENTQT_HEADERS include/FluentQt/Components/FluentChartView.h)
endif()

add_library(FluentQt SHARED ${FLUENTQT_SOURCES} ${FLUENTQT_HEADERS})

target_include_directories(FluentQt 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(FluentQt
    PUBLIC
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        Qt6::PrintSupport
)

# Conditionally link Qt Charts
if(FLUENT_CHARTS_AVAILABLE)
    target_link_libraries(FluentQt PUBLIC Qt6::Charts)
    target_compile_definitions(FluentQt PUBLIC FLUENT_CHARTS_AVAILABLE)
endif()

# Compiler-specific optimizations
if(MSVC)
    target_compile_options(FluentQt PRIVATE /W4 /O2)
else()
    target_compile_options(FluentQt PRIVATE -Wall -Wextra -O3)
endif()

# Example applications
add_executable(FluentQtDemo examples/main.cpp)
target_link_libraries(FluentQtDemo FluentQt)

# Carousel showcase example
add_executable(CarouselShowcase examples/CarouselShowcaseExample.cpp)
target_link_libraries(CarouselShowcase FluentQt)

# Timeline examples
add_executable(TimelineShowcase examples/TimelineShowcaseExample.cpp)
target_link_libraries(TimelineShowcase FluentQt)

add_executable(SimpleTimeline examples/SimpleTimelineExample.cpp)
target_link_libraries(SimpleTimeline FluentQt)

# Toast and Select demo
add_executable(ToastAndSelectDemo examples/ToastAndSelectDemo/main.cpp)
target_link_libraries(ToastAndSelectDemo FluentQt)

# Fluent Animation example
add_executable(FluentAnimationExample examples/FluentAnimationExample.cpp)
target_link_libraries(FluentAnimationExample FluentQt)

# Tests (will be added later)
# enable_testing()
# add_subdirectory(tests)