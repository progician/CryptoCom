#pragma once

#include <cmath>
#include <functional>
#include <utility>

namespace CryptoCom::ElGamal {
  namespace _Private {
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

      for (; exponent > 0; exponent >>= 1) {
        if (exponent & 1) {
          result = ModuloMul(result, base, m);
        }

        base = ModuloMul(base, base, m);
      }

      return result;
    }
  }
  

  struct ElGamal {
    static auto GenerateKeys(int order, int generator, std::function<int()>& rng) -> std::pair<int, int> {
      auto const private_key = rng();
      auto const public_key = _Private::ModuloPow(generator, private_key, order);
      return {private_key, public_key};
    }

    static auto Encrypt(int message, int key, int order, int generator, std::function<int()>& rng) -> std::pair<int, int> {
      using namespace _Private;
      auto const random_secret = rng();
      return {
          ModuloPow(generator, random_secret, order),
          ModuloMul(ModuloPow(key, random_secret, order), message, order)
      };
    }
  };

} // CryptoCom::ElGamal