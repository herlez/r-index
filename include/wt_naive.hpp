#pragma once

#include <assert.h>

#include <array>

#include "util/bit_vector.hpp"
#include "util/rank.hpp"

namespace alx {
inline size_t log2_of_uint64(uint64_t const x) {
  assert(x != 0);
  return 63 - __builtin_clzll(x);
}

class wavelet_tree {
 private:
  static constexpr uint64_t c_alphabet_size = 256;
  std::vector<uint8_t> m_alphabet;
  std::vector<BitVector> m_bvs;
  std::vector<Rank> m_bvrs;

  struct package {
    std::vector<uint8_t> text_in_node;
    uint8_t sigma_left = 0;   // inclusive
    uint8_t sigma_right = 0;  // exclusive

    package(uint8_t left, uint8_t right) : sigma_left(left), sigma_right(right) {}
  };

  template<typename C>
  std::vector<uint8_t> get_alphabet(C const& text) {
    std::array<bool, c_alphabet_size> hist{};
    for (auto c : text) {
      // assert(c < c_alphabet_size);
      hist[c] = true;
    }
    std::vector<uint8_t> alphabet;
    for (size_t i = 0; i < c_alphabet_size; ++i) {
      if (hist[i]) {
        alphabet.push_back(i);
      }
    }
    return alphabet;
  }

  uint8_t navigate_c(uint8_t c) {
    uint8_t result_c = 0;

    uint8_t left_sigma = 0;
    uint8_t right_sigma = m_alphabet.size();
    uint8_t middle_sigma;
    uint8_t middle_char;

    for (size_t l = 0; l < m_bvs.size(); ++l) {
      // std::cout << "<-" << size_t{left_sigma} << "-" << size_t{right_sigma} << "->" << "\n";
      middle_sigma = left_sigma + (right_sigma - left_sigma) / 2;
      middle_char = m_alphabet[middle_sigma];
      if (c < middle_char) {
        // std::cout << size_t{c} << "<" << size_t{middle_char} << "\n";
        result_c <<= 1;
        right_sigma = middle_sigma;
      } else {
        result_c <<= 1;
        ++result_c;
        left_sigma = middle_sigma;
      }
    }
    return result_c;
  }

 public:

  wavelet_tree() = default;
  
  template<typename C>
  wavelet_tree(C const& text) : m_alphabet(get_alphabet(text)) {
    // Build bit vectors
    std::vector<package> packages;
    packages.push_back(package{0, static_cast<uint8_t>(m_alphabet.size())});
    for (auto c : text) {
      packages[0].text_in_node.push_back(c);
    }

    size_t levels_needed = 1 + log2_of_uint64(m_alphabet.size() - 1);
    for (size_t level = 0; level < levels_needed; ++level) {
      m_bvs.push_back(BitVector(text.size()));
      size_t write_position = 0;
      std::vector<package> next_packages;

      for (auto& p : packages) {
        uint8_t middle_position = p.sigma_left + (p.sigma_right - p.sigma_left) / 2;
        uint8_t middle_sigma = m_alphabet[middle_position];
        package left_package{p.sigma_left, middle_position};
        package right_package{middle_position, p.sigma_right};
        for (auto c : p.text_in_node) {
          if (c < middle_sigma) {
            m_bvs[level].set_bit(write_position++, 0);
            left_package.text_in_node.push_back(c);
          } else {
            m_bvs[level].set_bit(write_position++, 1);
            right_package.text_in_node.push_back(c);
          }
        }
        next_packages.push_back(left_package);
        next_packages.push_back(right_package);
      }
      assert(next_packages.size() == packages.size() * 2);
      assert(write_position == text.size());
      packages = next_packages;
      // std::swap(packages, next_packages);
    }
    // Build rank data structure for bit vectors
    for (auto const& bvs : m_bvs) {
      m_bvrs.push_back(Rank(bvs));
    }
  }

  uint8_t operator[](size_t const pos) const {
    uint8_t alphabet_from = 0;
    uint8_t alphabet_to = m_alphabet.size();
    size_t left = 0;                  // inclusive
    size_t right = m_bvs[0].size();  // exclusive
    size_t pos_in_next_level = pos;

    for (size_t level = 0; level < m_bvrs.size(); ++level) {
      size_t bit = m_bvs[level].get_bit(pos_in_next_level);
      if (!bit) {
        alphabet_to = (alphabet_from + alphabet_to) / 2;
        size_t ones_in_node = (left == 0) ? m_bvrs[level].rank1(right - 1) : m_bvrs[level].rank1(right - 1) - m_bvrs[level].rank1(left - 1);
        // size_t rank0_in_node = (left == 0) ? m_bvrs[level]->rank0(pos_in_next_level) : m_bvrs[level]->rank0(pos_in_next_level) - m_bvrs[level]->rank0(left - 1);
        size_t rank1_in_node = (left == 0) ? m_bvrs[level].rank1(pos_in_next_level) : m_bvrs[level].rank1(pos_in_next_level) - m_bvrs[level].rank1(left - 1);
        right -= ones_in_node;
        // was delted before: pos_in_next_level = left + (rank0_in_node - 1);
        pos_in_next_level -= rank1_in_node;
      } else {
        alphabet_from = (alphabet_from + alphabet_to) / 2;
        size_t zeroes_in_node = (left == 0) ? m_bvrs[level].rank0(right - 1) : m_bvrs[level].rank0(right - 1) - m_bvrs[level].rank0(left - 1);
        size_t rank1_in_node = (left == 0) ? m_bvrs[level].rank1(pos_in_next_level) : m_bvrs[level].rank1(pos_in_next_level) - m_bvrs[level].rank1(left - 1);
        left += zeroes_in_node;
        pos_in_next_level = left + (rank1_in_node - 1);
      }
    }

    return m_alphabet[alphabet_from];
  }

  size_t rank(size_t pos, char c, bool debug = false) {
    size_t left = 0;                  // inclusive
    size_t right = m_bvs[0].size();  // exclusive
    size_t pos_in_next_level = pos;
    uint8_t code_c = navigate_c(c);

    if (debug) {
      std::cout << "\nrank(" << pos << ", " << std::hex << c << "=" << size_t{code_c} << std::dec << ")\n";
    };  // = pos_in_next_level - left;

    for (size_t level = 0; level < m_bvrs.size(); ++level) {
      size_t bit = (code_c >> (m_bvrs.size() - 1 - level)) & 1;

      if (debug) {
        std::cout << "<" << left << "-" << right << ">"
                  << "\n";
        std::cout << "Pos: " << pos_in_next_level << "\n";
        std::cout << "Next bit: " << bit << "\n\n";
      }

      if (!bit) {
        size_t ones_in_node = (left == 0) ? m_bvrs[level].rank1(right - 1) : m_bvrs[level].rank1(right - 1) - m_bvrs[level].rank1(left - 1);
        // size_t rank0_in_node = (left == 0) ? m_bvrs[level].rank0(pos_in_next_level) : m_bvrs[level].rank0(pos_in_next_level) - m_bvrs[level].rank0(left - 1);
        size_t rank1_in_node = (left == 0) ? m_bvrs[level].rank1(pos_in_next_level) : m_bvrs[level].rank1(pos_in_next_level) - m_bvrs[level].rank1(left - 1);
        right -= ones_in_node;
        // was delted before: pos_in_next_level = left + (rank0_in_node - 1);
        pos_in_next_level -= rank1_in_node;
      } else {
        size_t zeroes_in_node = (left == 0) ? m_bvrs[level].rank0(right - 1) : m_bvrs[level].rank0(right - 1) - m_bvrs[level].rank0(left - 1);
        size_t rank1_in_node = (left == 0) ? m_bvrs[level].rank1(pos_in_next_level) : m_bvrs[level].rank1(pos_in_next_level) - m_bvrs[level].rank1(left - 1);
        left += zeroes_in_node;
        pos_in_next_level = left + (rank1_in_node - 1);
      }
      if(pos_in_next_level < left) { return 0;}
    }

    if (debug) {
      std::cout << "<" << left << "-" << right << ">"
                << "\n";
      std::cout << "Pos: " << pos_in_next_level << "\n";
      std::cout << "Result: " << pos_in_next_level - left << "\n\n";
    }
    return 1 + pos_in_next_level - left;
  }

  void print_bvs(size_t max_symbols) {
    std::cout << "|Sigma|=" << m_alphabet.size() << ": ";
    for (auto c : m_alphabet) {
      std::cout << std::hex << "0x" << size_t{c} << ", ";
    }
    std::cout << "\nLevels: " << m_bvs.size() << " Width: " << std::dec << m_bvs[0].size() << "\n";
    for (BitVector bv : m_bvs) {
      for (size_t i = 0; i < std::min(bv.size(), max_symbols); ++i) {
        std::cout << bv.get_bit(i);
      }
      std::cout << '\n';
    }
    std::cout << '\n';
  }
};  // class ExampleClass1
}  // namespace alx