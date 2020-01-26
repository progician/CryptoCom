#include "catch2/catch.hpp"
#include "CryptoCom/ElGamal.h"
#include <iostream>

#include <limits>
#include <queue>


using namespace CryptoCom;

namespace CryptoCom {
  template<typename F>
  auto operator==(typename ElGamal<F>::Cipher const& lhs, typename ElGamal<F>::Cipher const& rhs) -> bool {
    return lhs.c1 == rhs.c1 && lhs.c2 == rhs.c2;
  }

  template<typename F>
  auto operator<<(std::ostream& ostr, typename ElGamal<F>::Cipher const& cipher) -> std::ostream& {
    ostr << "{" << cipher.c1 << ", " << cipher.c2 << "}";
    return ostr;
  }
}


struct F {
  static int const order = 8009;
  static int const generator = 1131;
};
using AgreedElGamal = ElGamal<F>;


TEST_CASE("Basic check for ElGamal crypto system") {
  constexpr auto const SecretNumber = 79;
  auto rng_for_key = std::function<int()>{[SecretNumber](){ return SecretNumber; }};


  auto const keys = AgreedElGamal::GenerateKeys(rng_for_key);
  REQUIRE(keys == std::make_pair(SecretNumber, 7697));

  SECTION("encoding") {
    constexpr auto const Message = 102;
    auto rng_for_encryption = std::function<int()>{[SecretNumber](){ return 90; }};
    auto cipher = AgreedElGamal::Encrypt(Message, keys.second, rng_for_encryption);
    auto expected = AgreedElGamal::Cipher{6603, 5571};
    REQUIRE(cipher == expected);

    SECTION("decoding") {
      auto decrypted = AgreedElGamal::Decrypt(cipher, keys.first);
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
  auto const keys = AgreedElGamal::GenerateKeys(rng);

  auto const five_e = AgreedElGamal::Encrypt(5, keys.second, rng);
  auto const four_e = AgreedElGamal::Encrypt(4, keys.second, rng);

  auto const multiplied_e = five_e * four_e;
  auto const result = AgreedElGamal::Decrypt(multiplied_e, keys.first);
  REQUIRE(result == 20);
}