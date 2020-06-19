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
using NormalElGamal = ElGamal<F>;


namespace Catch {
  template<> struct StringMaker<std::pair<int, int>> {
    static std::string convert(std::pair<int, int> const& p) {
      return fmt::format("({}, {})", p.first, p.second);
    }
  };

  template<> struct StringMaker<NormalElGamal::Cipher> {
    static std::string convert(NormalElGamal::Cipher const& c) {
      return fmt::format("({}, {})", c.c1, c.c2);
    }
  };
}


TEST_CASE("Basic check for ElGamal crypto system") {
  constexpr auto const SecretNumber = 79;
  auto rng_for_key = std::function<int()>{[SecretNumber](){ return SecretNumber; }};


  auto const keys = NormalElGamal::GenerateKeys(rng_for_key);
  REQUIRE(keys == std::make_pair(SecretNumber, 4097));

  SECTION("encoding") {
    constexpr auto const Message = 102;
    auto rng_for_encryption = std::function<int()>{[SecretNumber](){ return 90; }};
    auto cipher = NormalElGamal::Encrypt(Message, keys.second, rng_for_encryption);
    REQUIRE(cipher == NormalElGamal::Cipher{1132, 7773});

    SECTION("decoding") {
      auto decrypted = NormalElGamal::Decrypt(cipher, keys.first);
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

  auto const keys = NormalElGamal::GenerateKeys(rng);

  auto const five_e = NormalElGamal::Encrypt(5, keys.second, rng);
  auto const four_e = NormalElGamal::Encrypt(4, keys.second, rng);

  auto const multiplied_e = five_e * four_e;
  auto const result = NormalElGamal::Decrypt(multiplied_e, keys.first);
  REQUIRE(result == 20);
}



using ExpCrypto = ExpElGamal<F>;

TEST_CASE("ExpElGamal is additively homomorphic") {
  auto rng = std::function<int()>{[]() {
    static auto numbers = std::queue<int>({79, 90, 23});
    auto r = numbers.front();
    numbers.pop();
    return r;
  }};

  auto const keys = ExpCrypto::GenerateKeys(rng);
  auto const five_e = ExpCrypto::Encrypt(5, keys.second, rng);
  auto const four_e = ExpCrypto::Encrypt(4, keys.second, rng);

  auto const multiplied_e = five_e + four_e;
  auto const result = ExpCrypto::Decrypt(multiplied_e, keys.first);
  REQUIRE(result == ExpCrypto::Apply(5 + 4));
}