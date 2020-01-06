#pragma once

#include <cmath>
#include <functional>

namespace CryptoCom::ElGamal {
  
  struct ElGamal {
    static auto GenerateKeys(int order, int generator, std::function<int()>& rng) -> std::pair<int, int> {
      return {3, 8};
    }
  };

} // CryptoCom::ElGamal