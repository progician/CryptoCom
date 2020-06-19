from conans import CMake, ConanFile

class CryptoCom(ConanFile):
    name = "CryptoCom"
    description = "Library and application for Private Set Intersection"
    author = "Gyula Gubacsi <gyula.gubacsi@gmail.com>"
    
    requires = "catch2/[~2.12.2]", "fmt/[~6.2.1]"
    generators = "cmake_find_package"