#pragma once

#include <cmath>
#include <functional>
#include <tuple>
#include <utility>

namespace CryptoCom {

  auto Normalise(int n, int order) -> int {
    auto result = n % order;
    return result >= 0 ? result : result + order;
  }

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
      using namespace std;
      auto gcd = 0;
      auto x = 0;
      tie(gcd, x, ignore) = ExtendedGCD(n, m);

      if (gcd != 1) { 
        throw invalid_argument{"can't produce inverse, arguments are relative primes"};
      }

      return (x % m + m) % m;
    }


    auto ModuloMul(int lhs, int rhs, int m) -> int {
      auto result = 0;

      for (; rhs > 0; rhs >>= 1) {
        if (rhs % 2 == 1) {
          result += lhs;
          if (result > m) {
            result %= m;
          }
        }

        lhs <<= 1;
        if (lhs > m) {
          lhs %= m;
        }
      }

      return result;
    }


    auto ModuloPow(int base, int exponent, int m) -> int {
      using std::abs;
      auto result = 1;
      if (exponent < 0) {
        auto const p = ModuloPow(base, abs(exponent), m);
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


  template<typename Group> struct ElGamal {
    static auto GenerateKeys(std::function<int()>& rng) -> std::pair<int, int> {
      auto const private_key = rng();
      auto const public_key = _Private::ModuloPow(Group::generator, private_key, Group::order);
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
            ModuloMul(c1, rhs.c1, Group::order),
            ModuloMul(c2, rhs.c2, Group::order),
        };
      }

    };


    static auto Encrypt(int message, int key, std::function<int()>& rng) -> Cipher {
      using namespace _Private;
      auto const random_secret = rng();
      message = Normalise(message, Group::order);
      return {
          ModuloPow(Group::generator, random_secret, Group::order),
          ModuloMul(ModuloPow(key, random_secret, Group::order), message, Group::order),
      };
    }


    static auto Decrypt(Cipher const cipher, int key) -> int {
      using namespace _Private;
      auto inverse = ModuloPow(cipher.c1, -1 * key, Group::order);
      return ModuloMul(cipher.c2, inverse, Group::order);
    }

  };

  template<typename Group> struct ExpElGamal {
    using PlainElGamal = ElGamal<Group>;

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
            ModuloMul(c1, rhs.c1, Group::order),
            ModuloMul(c2, rhs.c2, Group::order),
        };
      }

      auto operator+=(Cipher const& rhs) -> Cipher& {
        *this = *this + rhs;
        return *this;
      }

      auto operator+(int rhs) const -> Cipher {
        using namespace _Private;
        return {
            c1,
            ModuloMul(
                c2,
                ModuloPow(Group::generator, Normalise(rhs, Group::order), Group::order),
                Group::order
            )
        };
      }

      auto operator-(int rhs) const -> Cipher {
        return *this + -rhs;
      }


      auto operator*(int rhs) const -> Cipher {
        using namespace _Private;
        auto const plain = Normalise(rhs, Group::order);
        return {
            ModuloPow(c1, plain, Group::order),
            ModuloPow(c2, plain, Group::order),
        };
      }
    };

    
    static auto Encrypt(int message, int key, std::function<int()>& rng) -> Cipher {
      using namespace _Private;
      auto const random_secret = rng();
      return {
          ModuloPow(Group::generator, random_secret, Group::order),
          ModuloMul(
              ModuloPow(key, random_secret, Group::order),
              ModuloPow(Group::generator, message, Group::order),
              Group::order
          ),
      };
    }


    static auto Decrypt(Cipher const cipher, int key) -> int {
      using namespace _Private;
      auto inverse = ModuloPow(cipher.c1, -1 * key, Group::order);
      return ModuloMul(cipher.c2, inverse, Group::order);
    }

    static auto Apply(int number) -> int {
      using namespace _Private;
      return ModuloPow(Group::generator, number, Group::order);
    }
  };
} // CryptoCom::ElGamal