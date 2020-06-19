#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>


#include "fmt/format.h"


#include "CryptoCom/ElGamal.h"


template<typename FieldT> using Polynomial = std::vector<FieldT>;

template<typename FieldT>
  auto operator*(Polynomial<FieldT> const& lhs, Polynomial<FieldT> const& rhs) -> Polynomial<FieldT> {
    Polynomial<FieldT> result;
    result.resize(lhs.size() + rhs.size() - 1, 0);
    for (int i = 0; i < lhs.size(); ++i) {
      for (int j = 0; j < rhs.size(); ++j) {
        result[i + j] += lhs[i] * rhs[j];
      }
    }
    return result;
  }

struct ElGamalTraits {
  static int const order = 8009;
  static int const generator = 1131;
};
using Crypto = CryptoCom::ExpElGamal<ElGamalTraits>;
using Field = Crypto::Cipher;


auto Random(int order) -> int {
  using namespace std;
  static auto rd = random_device{};
  static auto engine = mt19937{rd()};

  static auto distribution = uniform_int_distribution<int>{0, order - 1};
  return distribution(engine);
}
auto RandomInt() -> int { return Random(ElGamalTraits::order); }


template<typename Rng, typename It, typename Op>
  void Transform(Rng const& rng, It out, Op op) {
    using namespace std;
    transform(begin(rng), end(rng), out, op);
  }


template<typename Rng, typename V>
  bool Contains(Rng const& rng, V v) {
    using namespace std;
    return find(begin(rng), end(rng), v) != end(rng);
  }


auto Keys = Crypto::GenerateKeys(RandomInt);


auto Encrypt(int v) -> Crypto::Cipher {
  return Crypto::Encrypt(v, Keys.first, RandomInt);
};


auto Decrypt(Crypto::Cipher c) -> int {
  return Crypto::Decrypt(c, Keys.second);
};


auto prepare(std::vector<int> const& numbers) -> Polynomial<Field> {
  // Turn the numbers into coefficients of a polynomial with the roots of the set
  using namespace std;

  auto polynomial = Polynomial<int>{1};
  for (int i = 0; i < numbers.size(); i++) {
    polynomial = polynomial * Polynomial<int>{-1 * numbers[i], 1};
  }

  auto result = Polynomial<Crypto::Cipher>{};
  result.reserve(polynomial.size());

  Transform(polynomial, back_inserter(result), Encrypt);

  return result;
}


template<typename BidirIter, typename V>
  auto Eval(BidirIter begin, BidirIter end, V x) {
    assert(begin != end);
    auto result = *--end;

    while(end != begin) {
      result = result * x + *--end;
    }

    return result;
  }


template<typename Integral>
  auto evaluate(Polynomial<Field> const& poly, std::vector<Integral> const& numbers) -> std::vector<Field> {
    using namespace std;
    auto result = vector<Field>{};
    result.reserve(numbers.size());
    Transform(numbers, back_inserter(result),
        [&poly](auto x) { return Eval(begin(poly), end(poly), x) + x; }
    );
    return result;
  }


auto extract_intersection(std::vector<Field> const& evaluated_set, std::vector<int> const& original) -> std::vector<int> {
  using namespace std;

  auto decrypted = vector<int>{};
  decrypted.reserve(evaluated_set.size());
  Transform(evaluated_set, back_inserter(decrypted), Decrypt);

  auto original_pow = vector<int>{};
  original_pow.reserve(original.size());
  Transform(original, back_inserter(original_pow),
      [](int x) {
        using namespace CryptoCom::_Private;
        return ModuloPow(ElGamalTraits::generator, x, ElGamalTraits::order);
      }
  );

  auto intersection = vector<int>{};
  for (auto i = 0u; i < decrypted.size(); ++i) {
    auto const elem_in_local = find(begin(original_pow), end(original_pow), decrypted[i]);
    if (elem_in_local != end(original_pow)) {
      intersection.push_back(*elem_in_local);
    }
  }
  return intersection;
}


auto operator<<(std::ostream& ostr, std::vector<int> const& numbers) -> std::ostream& {
  std::copy(
    numbers.begin(), numbers.end(),
    std::ostream_iterator<int>(ostr, ", ")
  );
  return ostr;
}


auto main(int, char const*[]) -> int {
  auto const local = std::vector<int>{11, 13};
  auto const encoded = prepare(local);

  auto const remote = std::vector<int>{10, 11};
  auto const evaluated = evaluate(encoded, remote);

  auto const actual = extract_intersection(evaluated, local);
  auto const expected = std::vector<int>{11};
  
  if (actual.size() != expected.size() || !std::equal(actual.begin(), actual.end(), expected.begin())) {
    fmt::print(stderr, "failed: sets are not equal!\n");
    fmt::print(stderr, "\texpected: {}\n", fmt::join(expected, ", "));
    fmt::print(stderr, "\tactual:   {}\n", fmt::join(actual, ", "));
    return 1;
  }

  fmt::print("passed\n");
  return 0;
}