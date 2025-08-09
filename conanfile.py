from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy, get, save
from conan.tools.scm import Git
import os


class FluentQtConan(ConanFile):
    name = "fluentqt"
    version = "1.0.0"
    description = "A modern, comprehensive C++ Qt6 library implementing Microsoft's Fluent Design System"
    license = "MIT"
    author = "Max Qian <astro_air@126.com>"
    url = "https://github.com/ElementAstro/element-fluent-ui"
    homepage = "https://github.com/ElementAstro/element-fluent-ui"
    topics = ("qt6", "cpp", "fluent-design", "ui-library", "components")

    # Package configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "charts": [True, False],
        "multimedia": [True, False],
        "network": [True, False],
        "examples": [True, False],
        "tests": [True, False],
        "docs": [True, False]
    }
    default_options = {
        "shared": True,
        "fPIC": True,
        "charts": True,
        "multimedia": False,
        "network": False,
        "examples": False,
        "tests": False,
        "docs": False
    }

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "LICENSE", "README.md"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        # Core Qt6 requirements
        self.requires("qt/6.5.0")
        
        # Optional Qt6 components
        if self.options.charts:
            # Qt Charts is typically included in the main Qt package
            pass
        
        if self.options.multimedia:
            # Qt Multimedia is typically included in the main Qt package
            pass
        
        if self.options.network:
            # Qt Network is typically included in the main Qt package
            pass

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.20]")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        
        tc = CMakeToolchain(self)
        
        # Configure CMake options based on Conan options
        tc.variables["FLUENTQT_BUILD_SHARED"] = self.options.shared
        tc.variables["FLUENTQT_ENABLE_CHARTS"] = self.options.charts
        tc.variables["FLUENTQT_ENABLE_MULTIMEDIA"] = self.options.multimedia
        tc.variables["FLUENTQT_ENABLE_NETWORK"] = self.options.network
        tc.variables["FLUENTQT_BUILD_EXAMPLES"] = self.options.examples
        tc.variables["FLUENTQT_BUILD_TESTS"] = self.options.tests
        tc.variables["FLUENTQT_BUILD_DOCS"] = self.options.docs
        tc.variables["FLUENTQT_INSTALL"] = True
        
        # Ensure C++20 standard
        tc.variables["CMAKE_CXX_STANDARD"] = "20"
        tc.variables["CMAKE_CXX_STANDARD_REQUIRED"] = True
        
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, "LICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        # Main library
        self.cpp_info.libs = ["FluentQt"]
        
        # Include directories
        self.cpp_info.includedirs = ["include"]
        
        # Compiler definitions
        self.cpp_info.defines = [
            f"FLUENTQT_VERSION_MAJOR={self.version.split('.')[0]}",
            f"FLUENTQT_VERSION_MINOR={self.version.split('.')[1]}",
            f"FLUENTQT_VERSION_PATCH={self.version.split('.')[2]}",
            f"FLUENTQT_VERSION_STRING=\"{self.version}\""
        ]
        
        if self.options.shared:
            self.cpp_info.defines.append("FLUENTQT_SHARED")
        else:
            self.cpp_info.defines.append("FLUENTQT_STATIC")
        
        if self.options.charts:
            self.cpp_info.defines.append("FLUENTQT_CHARTS_AVAILABLE")
        
        if self.options.multimedia:
            self.cpp_info.defines.append("FLUENTQT_MULTIMEDIA_AVAILABLE")
        
        if self.options.network:
            self.cpp_info.defines.append("FLUENTQT_NETWORK_AVAILABLE")
        
        # Platform-specific settings
        if self.settings.os == "Windows":
            self.cpp_info.system_libs.extend(["dwmapi", "user32", "gdi32"])
        elif self.settings.os == "Macos":
            self.cpp_info.frameworks.append("Cocoa")
        elif self.settings.os == "Linux":
            # Linux-specific libraries if needed
            pass
        
        # CMake configuration
        self.cpp_info.set_property("cmake_file_name", "FluentQt")
        self.cpp_info.set_property("cmake_target_name", "FluentQt::FluentQt")
        
        # pkg-config configuration
        self.cpp_info.set_property("pkg_config_name", "FluentQt")

    def package_id(self):
        # Make package ID independent of certain options that don't affect the binary
        if self.options.examples:
            del self.info.options.examples
        if self.options.tests:
            del self.info.options.tests
        if self.options.docs:
            del self.info.options.docs
