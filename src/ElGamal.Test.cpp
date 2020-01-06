#include "catch2/catch.hpp"
#include "CryptoCom/ElGamal.h"

#include <limits>

using namespace CryptoCom;

TEST_CASE("Hello, World!") {
  auto rng = std::function<int()>{[](){ return 3; }};
  auto const keys = ElGamal::ElGamal::GenerateKeys(std::numeric_limits<int>::max(), 2, rng);
  REQUIRE(keys.first == 3);
  REQUIRE(keys.second == 8);
}