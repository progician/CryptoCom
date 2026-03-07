#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>


#include "fmt/format.h"
#include "fmt/ranges.h"


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
std::function<int()> RandomInt = []() -> int { return Random(ElGamalTraits::order); };


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
  // encryption should use the *public* key (second element)
  return Crypto::Encrypt(v, Keys.second, RandomInt);
};


auto Decrypt(Crypto::Cipher c) -> int {
  // decryption requires the private key (first element)
  return Crypto::Decrypt(c, Keys.first);
};


template<> struct fmt::formatter<Crypto::Cipher> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
    auto format(Crypto::Cipher const& c, FormatContext& ctx) const {
      return format_to(ctx.out(), "({}, {})", c.c1, c.c2);
    }
};


auto prepare(std::vector<int> const& numbers) -> Polynomial<Field> {
  // Turn the numbers into coefficients of a polynomial with the roots of the set
  using namespace std;

  auto polynomial = Polynomial<int>{1};
  for (int i = 0; i < numbers.size(); i++) {
    polynomial = polynomial * Polynomial<int>{-1 * numbers[i], 1};
  }
  fmt::print("polynomial: {}\n", fmt::join(polynomial, ", "));

  auto result = Polynomial<Crypto::Cipher>{};
  result.reserve(polynomial.size());

  Transform(polynomial, back_inserter(result), Encrypt);

  return result;
}


template<typename BidirIter, typename V>
  auto Eval(BidirIter begin, BidirIter end, V x) {
    assert(begin != end);
    auto result = *--end;

    while (end != begin) {
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


auto extract_intersection(std::vector<Field> const& evaluated_set,
                          std::vector<int> const& original,
                          std::vector<int> const& remote) -> std::vector<int> {
  using namespace std;

  auto references = vector<int>{};
  Transform(remote, std::back_inserter(references), Crypto::Apply);

  auto evaluated_decrypted = vector<int>{};
  Transform(evaluated_set, std::back_inserter(evaluated_decrypted), Decrypt);

  auto intersection = vector<int>{};
  intersection.reserve(min(evaluated_set.size(), std::max(original.size(), remote.size())));

  for (auto idx = 0u; idx < evaluated_set.size() && idx < remote.size(); ++idx) {
    int const x = remote[idx];
    auto decrypted = evaluated_decrypted[idx];
    auto reference = references[idx];

    fmt::print("\tdecrypted[{}] = {} (expect {} for {})\n",
               idx, decrypted, reference, x);

    if (decrypted == reference) {
      // x was in the local sete
      intersection.push_back(x);
    }
  }

  return intersection;
}


auto main(int, char const*[]) -> int {
  auto const local = std::vector<int>{11, 13};
  auto const encoded = prepare(local);
  fmt::print("\tencoded: {}\n", fmt::join(encoded, ", "));

  auto const remote = std::vector<int>{10, 11};
  auto const evaluated = evaluate(encoded, remote);
  fmt::print("\tevaluated: {}\n", fmt::join(evaluated, ", "));

  auto const actual = extract_intersection(evaluated, local, remote);
  auto const expected = std::vector<int>{11};
  
  if (actual.size() != expected.size() || !std::equal(actual.begin(), actual.end(), expected.begin())) {
    fmt::print(stderr, "failed: sets are not equal!\n\texpected: {}\n\tactual:  {}\n",
        fmt::join(expected, ", "),
        fmt::join(actual, ", ")
    );
    return 1;
  }

  fmt::print("passed\n");
  return 0;
}