#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <vector>


auto operator*(std::vector<int> const& lhs, std::vector<int> const& rhs) -> std::vector<int> {
  std::vector<int> result;
  result.resize(lhs.size() + rhs.size() - 1, 0);
  for (int i = 0; i < lhs.size(); ++i) {
    for (int j = 0; j < rhs.size(); ++j) {
      result[i + j] += lhs[i] * rhs[j];
    }
  }
  return result;
}


auto prepare(std::vector<int> const& numbers) -> std::vector<int> {
  // Turn the numbers into coefficients of a polynomial with the roots of the set
  auto result = std::vector<int>{1};
  for (int i = 0; i < numbers.size(); i++) {
    result = result * std::vector{-1 * numbers[i], 1};
  }
  return result;
}


auto evaluate(std::vector<int> const& polynomial, std::vector<int> const& set) -> std::vector<int> {
  auto result = std::vector<int>{};
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


auto extract_intersection(std::vector<int> const& evaluated_set, std::vector<int> const& original) -> std::vector<int> {
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