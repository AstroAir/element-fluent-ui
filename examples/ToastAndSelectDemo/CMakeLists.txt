# examples/ToastAndSelectDemo/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

project(ToastAndSelectDemo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Find FluentQt
find_package(FluentQt REQUIRED)

# Create executable
add_executable(ToastAndSelectDemo
    main.cpp
)

# Link libraries
target_link_libraries(ToastAndSelectDemo
    Qt6::Core
    Qt6::Widgets
    FluentQt::FluentQt
)

# Set target properties
set_target_properties(ToastAndSelectDemo PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Copy resources if they exist
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources"
         DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# Install target
install(TARGETS ToastAndSelectDemo
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
