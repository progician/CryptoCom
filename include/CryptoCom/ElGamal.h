#pragma once

#include <cmath>
#include <functional>
#include <tuple>
#include <utility>

namespace CryptoCom::ElGamal {
  namespace _Private {
    auto ExtendedGCD(int lhs, int rhs) -> std::tuple<int, int, int> {
      if (lhs == 0) {
        return {rhs, 0, 1};
      }

      auto [gcd, x, y] = ExtendedGCD(rhs % lhs, lhs);
      return {
        gcd,
        y - (rhs / lhs) * x,
        x
      };
    }


    auto ModuloInverse(int n, int m) -> int {
      auto gcd = 0;
      auto x = 0;
      std::tie(gcd, x, std::ignore) = ExtendedGCD(n, m);

      if (gcd != 1)
        throw std::invalid_argument{"can't produce inverse, arguments are relative primes"};

      return (x % m + m) % m;
    }


    auto ModuloMul(int lhs, int rhs, int m) -> int {
      auto result = 0;

      for (; lhs > 0; lhs >>= 1) {
        if (lhs & 1) {
          result += rhs;
          if (result > m) {
            result %= m;
          }
        }

        rhs <<= 1;
        if (rhs > m) {
          rhs %= m;
        }
      }

      return result;
    }


    auto ModuloPow(int base, int exponent, int m) -> int {
      auto result = 1;
      if (exponent < 0) {
        auto const p = ModuloPow(base, std::abs(exponent), m);
        return ModuloInverse(p, m);
      }

      for (; exponent > 0; exponent >>= 1) {
        if (exponent & 1) {
          result = ModuloMul(result, base, m);
        }

        base = ModuloMul(base, base, m);
      }

      return result;
    }
  }


  auto GenerateKeys(int order, int generator, std::function<int()>& rng) -> std::pair<int, int> {
    auto const private_key = rng();
    auto const public_key = _Private::ModuloPow(generator, private_key, order);
    return {private_key, public_key};
  }


  struct Cipher {
    int c1, c2;
    int order, generator;
  };


  auto Encrypt(int message, int key, int order, int generator, std::function<int()>& rng) -> Cipher {
    using namespace _Private;
    auto const random_secret = rng();
    return {
        ModuloPow(generator, random_secret, order),
        ModuloMul(ModuloPow(key, random_secret, order), message, order),
        order, generator
    };
  }


  auto Decrypt(Cipher const cipher, int key, int order, int generator) -> int {
    using namespace _Private;
    auto inverse = ModuloPow(cipher.c1, -1 * key, order);
    return ModuloMul(cipher.c2, inverse, order);
  }


  auto operator*(Cipher const& lhs, Cipher const& rhs) -> Cipher {
    using namespace _Private;
    return {
        ModuloMul(lhs.c1, rhs.c1, lhs.order),
        ModuloMul(lhs.c2, rhs.c2, lhs.order),
        lhs.order, lhs.generator
    };
  }
} // CryptoCom::ElGamal