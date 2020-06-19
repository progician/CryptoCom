#include "catch2/catch.hpp"
#include "CryptoCom/ElGamal.h"

#include "fmt/format.h"

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
  static int const generator = 1151;
};
using SimpleCrypto = ElGamal<F>;


namespace Catch {
  template<> struct StringMaker<std::pair<int, int>> {
    static std::string convert(std::pair<int, int> const& p) {
      return fmt::format("({0}, {1})", p.first, p.second);
    }
  };

  template<> struct StringMaker<SimpleCrypto::Cipher> {
    static std::string convert(SimpleCrypto::Cipher const& c) {
      return fmt::format("({0}, {1})", c.c1, c.c2);
    }
  };
}


TEST_CASE("Basic check for ElGamal crypto system") {
  constexpr auto const SecretNumber = 79;
  auto rng_for_key = std::function<int()>{[SecretNumber](){ return SecretNumber; }};


  auto const keys = SimpleCrypto::GenerateKeys(rng_for_key);
  REQUIRE(keys == std::make_pair(SecretNumber, 4097));

  SECTION("encoding") {
    constexpr auto const Message = 102;
    auto rng_for_encryption = std::function<int()>{[SecretNumber](){ return 90; }};
    auto cipher = SimpleCrypto::Encrypt(Message, keys.second, rng_for_encryption);
    REQUIRE(cipher == SimpleCrypto::Cipher{1132, 7773});

    SECTION("decoding") {
      auto decrypted = SimpleCrypto::Decrypt(cipher, keys.first);
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
  auto const keys = SimpleCrypto::GenerateKeys(rng);

  auto const five_e = SimpleCrypto::Encrypt(5, keys.second, rng);
  auto const four_e = SimpleCrypto::Encrypt(4, keys.second, rng);

  auto const multiplied_e = five_e * four_e;
  auto const result = SimpleCrypto::Decrypt(multiplied_e, keys.first);
  REQUIRE(result == 20);
}



using SimpleAdditiveCrypto = ExpElGamal<F>;

TEST_CASE("ExpElGamal is additively homomorphic") {
  auto rng = std::function<int()>{[]() {
    static auto numbers = std::queue<int>({79, 90, 23});
    auto r = numbers.front();
    numbers.pop();
    return r;
  }};

  constexpr auto const Order = 8009;
  constexpr auto const Generator = 1033;
  auto const keys = SimpleAdditiveCrypto::GenerateKeys(rng);

  auto const five_e = SimpleAdditiveCrypto::Encrypt(5, keys.second, rng);
  auto const four_e = SimpleAdditiveCrypto::Encrypt(4, keys.second, rng);

  auto const multiplied_e = five_e + four_e;
  auto const result = SimpleAdditiveCrypto::Decrypt(multiplied_e, keys.first);
  REQUIRE(result == 20);
}