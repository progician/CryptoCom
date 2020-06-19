cmake_minimum_required(VERSION 3.15)
project(CryptoCom
    VERSION 0.1.0
    DESCRIPTION "Private Set Intersection based communication"
)


#===-------------------------------------------------------------------------===
# For conan's find package modules
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})


#===-------------------------------------------------------------------------===
# Sanitizers
option(UBSAN "Using the Undefined Behaviour Sanitizer" OFF)
if (UBSAN)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=undefined")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=undefined")
endif()


#===-------------------------------------------------------------------------===
# CryptoCom library
add_library(CryptoCom INTERFACE)
add_library(CryptoCom::CryptoCom ALIAS CryptoCom)
target_include_directories(CryptoCom INTERFACE include)
target_compile_features(CryptoCom INTERFACE cxx_std_17)


#===-------------------------------------------------------------------------===
# Unit testing
find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)
add_executable(CryptoCom.UT EXCLUDE_FROM_ALL)
target_sources(CryptoCom.UT
    PRIVATE
        src/ElGamal.Test.cpp
        src/Main.Test.cpp
)
target_link_libraries(CryptoCom.UT
    PRIVATE
        Catch2::Catch2
        CryptoCom::CryptoCom
        fmt::fmt
)

add_custom_target(check-unit
    COMMAND $<TARGET_FILE:CryptoCom.UT> --use-colour yes
    COMMENT "Running unit tests..."
    DEPENDS CryptoCom.UT
)


#===-------------------------------------------------------------------------===
# Integration testing
add_executable(RoundTrip.Test EXCLUDE_FROM_ALL)
target_sources(RoundTrip.Test PRIVATE test/RoundTrip.Test.cpp)
target_compile_features(RoundTrip.Test PRIVATE cxx_std_17)
target_link_libraries(RoundTrip.Test PRIVATE CryptoCom::CryptoCom fmt::fmt)

add_custom_target(check-integration
    COMMAND $<TARGET_FILE:RoundTrip.Test>
    COMMENT "Running integration tests..."
    DEPENDS RoundTrip.Test
)