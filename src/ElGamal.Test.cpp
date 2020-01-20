#include "catch2/catch.hpp"
#include "CryptoCom/ElGamal.h"
#include <iostream>

#include <limits>
#include <queue>


using namespace CryptoCom;

namespace CryptoCom::ElGamal {
  auto operator==(Cipher const& lhs, Cipher const& rhs) -> bool {
    return lhs.c1 == rhs.c1 && lhs.c2 == rhs.c2;
  }

  auto operator<<(std::ostream& ostr, Cipher const& cipher) -> std::ostream& {
    ostr << "{" << cipher.c1 << ", " << cipher.c2 << "}";
    return ostr;
  }
}


TEST_CASE("Basic check for ElGamal crypto system") {
  constexpr auto const SecretNumber = 79;
  auto rng_for_key = std::function<int()>{[SecretNumber](){ return SecretNumber; }};

  constexpr auto const Order = 8009;
  constexpr auto const Generator = 1131;
  auto const keys = ElGamal::GenerateKeys(Order, Generator, rng_for_key);
  REQUIRE(keys == std::make_pair(SecretNumber, 7697));

  SECTION("encoding") {
    constexpr auto const Message = 102;
    auto rng_for_encryption = std::function<int()>{[SecretNumber](){ return 90; }};
    auto cipher = ElGamal::Encrypt(Message, keys.second, Order, Generator, rng_for_encryption);
    REQUIRE(cipher == ElGamal::Cipher{6603, 5571});

    SECTION("decoding") {
      auto decrypted = ElGamal::Decrypt(cipher, keys.first, Order, Generator);
      REQUIRE(decrypted == Message);
    }
  }
}


TEST_CASE("ElGamal is multiplicatively homomorphic") {
  auto rng = std::function<int()>{[]() {
    static auto numbers = std::queue<int>({79, 90, 23});
    auto r = numbers.front();
    numbers.pop();
    return r;
  }};

  constexpr auto const Order = 8009;
  constexpr auto const Generator = 1033;
  auto const keys = ElGamal::GenerateKeys(Order, Generator, rng);

  auto const five_e = ElGamal::Encrypt(5, keys.second, Order, Generator, rng);
  auto const four_e = ElGamal::Encrypt(4, keys.second, Order, Generator, rng);

  auto const multiplied_e = five_e * four_e;
  auto const result = ElGamal::Decrypt(multiplied_e, keys.first, Order, Generator);
  REQUIRE(result == 20);
}
