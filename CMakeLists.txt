cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(CryptoCom
    VERSION 0.1.0
    DESCRIPTION "Private Set Intersection based communication"
)

add_executable(RoundTrip.Test)
target_sources(RoundTrip.Test PRIVATE test/RoundTrip.Test.cpp)
target_compile_features(RoundTrip.Test PRIVATE cxx_std_17)