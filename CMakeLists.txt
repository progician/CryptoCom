cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(CryptoCom
    VERSION 0.1.0
    DESCRIPTION "Private Set Intersection based communication"
)


#===------------------------------------------------------------------------===
# Unit testing
find_package(Catch2 CONFIG REQUIRED)
add_executable(CryptoCom.UT EXCLUDE_FROM_ALL)
target_sources(CryptoCom.UT
    PRIVATE
        src/ElGamal.Test.cpp
        src/Main.Test.cpp
)
target_link_libraries(CryptoCom.UT PRIVATE Catch2::Catch2)

add_custom_target(check-unit
    COMMAND $<TARGET_FILE:CryptoCom.UT>
    COMMENT "Running unit tests..."
    DEPENDS CryptoCom.UT
)


#===------------------------------------------------------------------------===
# Integration testing
add_executable(RoundTrip.Test EXCLUDE_FROM_ALL)
target_sources(RoundTrip.Test PRIVATE test/RoundTrip.Test.cpp)
target_compile_features(RoundTrip.Test PRIVATE cxx_std_17)

add_custom_target(check-integration
    COMMAND $<TARGET_FILE:RoundTrip.Test>
    COMMENT "Running integration tests..."
    DEPENDS RoundTrip.Test
)