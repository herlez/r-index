#pragma once

#include <assert.h>

#include <array>

#include "bit_vector.hpp"
#include "rank.hpp"

inline size_t log2_of_uint64(uint64_t const x) {
  assert(x != 0);
  return 63 - __builtin_clzll(x);
}

class WaveletTree {
 private:
  static constexpr uint64_t c_alphabet_size = 256;
  std::vector<uint8_t> m_alphabet;
  std::vector<BitVector*> m_bvs;
  std::vector<Rank*> m_bvrs;

  struct package {
    std::vector<uint8_t> text_in_node;
    uint8_t sigma_left = 0;   //inclusive
    uint8_t sigma_right = 0;  //exclusive

    package(uint8_t left, uint8_t right) : sigma_left(left), sigma_right(right) {}
  };

  std::vector<uint8_t> get_alphabet(std::string_view text) {
    std::array<bool, c_alphabet_size> hist{};
    /*
    for(auto b : hist) {
      assert(!b);
    }
    */
    for (auto c : text) {
      assert(c < c_alphabet_size);
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

  void print_bvs() {
    for(auto bv : m_bvs) {
      for(size_t i = 0; i < bv[0].size(); ++i) {
        std::cout << bv->get_bit(i);
      }
      std::cout << '\n';
    }
    std::cout << '\n';
  }
 public:
  WaveletTree(std::string_view text) : m_alphabet(get_alphabet(text)) {
    //Build bit vectors
    std::vector<package> packages;
    packages.push_back(package{0, static_cast<uint8_t>(m_alphabet.size())});
    for (auto c : text) {
      packages[0].text_in_node.push_back(c);
    }

    
    size_t levels_needed = 1 + log2_of_uint64(m_alphabet.size() - 1);
    for (size_t level = 0; level < levels_needed; ++level) {
      m_bvs.push_back(new BitVector(text.size()));
      size_t write_position = 0;
      std::vector<package> next_packages;

      for (auto& p : packages) {
        uint8_t middle_position = p.sigma_left + (p.sigma_right - p.sigma_left) / 2;
        uint8_t middle_sigma = m_alphabet[middle_position];
        package left_package{p.sigma_left, middle_position};
        package right_package{middle_position, p.sigma_right};
        for (auto c : p.text_in_node) {
          if (c < middle_sigma) {
            m_bvs[level]->set_bit(write_position++, 0);
            left_package.text_in_node.push_back(c);
          } else {
            m_bvs[level]->set_bit(write_position++, 1);
            right_package.text_in_node.push_back(c);
          }
        }
        next_packages.push_back(left_package);
        next_packages.push_back(right_package);
      }
      assert(next_packages.size() == packages.size() * 2);
      assert(write_position == text.size());
      packages = next_packages;
      //std::swap(packages, next_packages);
    }
    // Build rank data structure for bit vectors
    for (auto& bvs : m_bvs) {
      m_bvrs.push_back(new Rank(*bvs));
    }
  }

  uint8_t operator[](size_t const pos) const {
    uint8_t alphabet_from = 0;
    uint8_t alphabet_to = m_alphabet.size();
    size_t left = 0;                 //inclusive
    size_t right = m_bvs[0]->size();  //exclusive
    size_t pos_in_next_level = pos;

    for (size_t level = 0; level < m_bvrs.size(); ++level) {
      size_t bit = m_bvs[level]->get_bit(pos_in_next_level);
      if (!bit) {
        alphabet_to = (alphabet_from+alphabet_to) / 2;
        size_t ones_in_node = (left == 0) ? m_bvrs[level]->rank1(right - 1) : m_bvrs[level]->rank1(right - 1) - m_bvrs[level]->rank1(left - 1);
        size_t rank0_in_node = (left == 0) ? m_bvrs[level]->rank0(pos_in_next_level) : m_bvrs[level]->rank0(pos_in_next_level) - m_bvrs[level]->rank0(left - 1);
        size_t rank1_in_node = (left == 0) ? m_bvrs[level]->rank1(pos_in_next_level) : m_bvrs[level]->rank1(pos_in_next_level) - m_bvrs[level]->rank1(left - 1);
        right -= ones_in_node;
        //pos_in_next_level = left + (rank0_in_node - 1);
        pos_in_next_level -= rank1_in_node;
      } else {
        alphabet_from = (alphabet_from+alphabet_to) / 2;
        size_t zeroes_in_node = (left == 0) ? m_bvrs[level]->rank0(right - 1) : m_bvrs[level]->rank0(right - 1) - m_bvrs[level]->rank0(left - 1);
        size_t rank1_in_node = (left == 0) ? m_bvrs[level]->rank1(pos_in_next_level) : m_bvrs[level]->rank1(pos_in_next_level) - m_bvrs[level]->rank1(left - 1);
        left += zeroes_in_node;
        pos_in_next_level = left + (rank1_in_node - 1);
      }
    }

    return m_alphabet[alphabet_from];
  }

};  // class ExampleClass1
