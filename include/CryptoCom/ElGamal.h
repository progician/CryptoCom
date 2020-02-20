#pragma once

#include <cmath>
#include <functional>
#include <tuple>
#include <utility>

namespace CryptoCom {
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

      if (gcd != 1) { 
        throw std::invalid_argument{"can't produce inverse, arguments are relative primes"};
      }

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


  template<typename Field> struct ElGamal {
    static auto GenerateKeys(std::function<int()>& rng) -> std::pair<int, int> {
      auto const private_key = rng();
      auto const public_key = _Private::ModuloPow(Field::generator, private_key, Field::order);
      return {private_key, public_key};
    }


    struct Cipher {
      int c1, c2;

      auto operator==(Cipher const& rhs) const -> bool {
        return c1 == rhs.c1 && c2 == rhs.c2;
      }

      auto operator*(Cipher const& rhs) const -> Cipher {
        using namespace _Private;
        return {
            ModuloMul(c1, rhs.c1, Field::order),
            ModuloMul(c2, rhs.c2, Field::order),
        };
      }

    };


    static auto Encrypt(int message, int key, std::function<int()>& rng) -> Cipher {
      using namespace _Private;
      auto const random_secret = rng();
      return {
          ModuloPow(Field::generator, random_secret, Field::order),
          ModuloMul(ModuloPow(key, random_secret, Field::order), message, Field::order),
      };
    }


    static auto Decrypt(Cipher const cipher, int key) -> int {
      using namespace _Private;
      auto inverse = ModuloPow(cipher.c1, -1 * key, Field::order);
      return ModuloMul(cipher.c2, inverse, Field::order);
    }

  };

  template<typename Field> struct ExpElGamal {
    using PlainElGamal = ElGamal<Field>;

    static auto GenerateKeys(std::function<int()>& rng) -> std::pair<int, int> {
      return PlainElGamal::GenerateKeys(rng);
    }


    struct Cipher {
      int c1, c2;

      auto operator==(Cipher const& rhs) const -> bool {
        return c1 == rhs.c1 && c2 == rhs.c2;
      }

      auto operator+(Cipher const& rhs) const -> Cipher {
        using namespace _Private;
        return {
            ModuloMul(c1, rhs.c1, Field::order),
            ModuloMul(c2, rhs.c2, Field::order),
        };
      }

      auto operator*(int rhs) const -> Cipher {
        using namespace _Private;
        return {
            c1,
            ModuloMul(
                c2,
                ModuloPow(Field::generator, rhs, Field::order)
            )
        };
      }
    };


    static auto Encrypt(int message, int key, std::function<int()>& rng) -> Cipher {
      using namespace _Private;
      auto const random_secret = rng();
      return {
          ModuloPow(Field::generator, random_secret, Field::order),
          ModuloMul(ModuloPow(key, random_secret, Field::order), message, Field::order),
      };
    }


    static auto Decrypt(Cipher const cipher, int key) -> int {
      using namespace _Private;
      auto inverse = ModuloPow(cipher.c1, -1 * key, Field::order);
      return ModuloMul(cipher.c2, inverse, Field::order);
    }
  };
} // CryptoCom::ElGamal