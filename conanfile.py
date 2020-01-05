from conans import CMake, ConanFile

class CryptoCom(ConanFile):
    name = "CryptoCom"
    description = "Library and application for Private Set Intersection"
    author = "Gyula Gubacsi <gyula.gubacsi@gmail.com>"
    
    requires = "Catch2/[~2.11.1]@catchorg/stable"