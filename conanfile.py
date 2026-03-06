from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout

class CryptoCom(ConanFile):
    name = "CryptoCom"
    version = "0.1"
    description = "Library and application for Private Set Intersection"
    author = "Gyula Gubacsi <gyula.gubacsi@gmail.com>"
    
    settings = "os", "compiler", "build_type", "arch"
    
    requires = "catch2/3.13.0", "fmt/12.1.0"
    
    generators = "CMakeDeps"
    
    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()