#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <vector>


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


using Field = int;


auto prepare(std::vector<int> const& numbers) -> Polynomial<Field> {
  // Turn the numbers into coefficients of a polynomial with the roots of the set
  auto result = Polynomial<int>{1};
  for (int i = 0; i < numbers.size(); i++) {
    result = result * Polynomial<int>{-1 * numbers[i], 1};
  }

  return result;
}


template<typename Integral>
  auto evaluate(Polynomial<Field> const& polynomial, std::vector<Integral> const& set) -> std::vector<Field> {
    auto result = std::vector<Integral>{};
    for (auto i = 0u; i < set.size(); ++i) {
      auto s = 0;
      for (auto j = 0u; j < polynomial.size(); ++j) {
        auto const x_pow = std::pow(set[i], j);
        s += polynomial[j] * x_pow;
      }
      result.push_back(s + set[i]);
    }
    return result;
  }


auto extract_intersection(std::vector<Field> const& evaluated_set, std::vector<int> const& original) -> std::vector<int> {
  auto intersection = std::vector<int>{};
  for (auto i = 0u; i < evaluated_set.size(); ++i) {
    auto const elem_in_local = std::find(
        original.begin(), original.end(),
        evaluated_set[i]
    );
    if (elem_in_local != original.end()) {
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
  auto const local = std::vector<int>{3, 5, 7, 11, 13, 17, 19};
  auto const encoded = prepare(local);

  auto const remote = std::vector<int>{9, 10, 11, 12, 13};
  auto const evaluated = evaluate(encoded, remote);

  auto const actual = extract_intersection(evaluated, local);
  auto const expected = std::vector<int>{11, 13};
  
  if (actual.size() != expected.size() || !std::equal(actual.begin(), actual.end(), expected.begin())) {
    std::cerr << "failed: sets are not equal!" << std::endl;
    std::cerr << "expected: " << expected << std::endl;
    std::cerr << "actual: " << actual << std::endl;
    return 1;
  }

  std::cout << "passed" << std::endl;
  return 0;
}