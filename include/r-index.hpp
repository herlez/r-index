#pragma once

#include <array>
#include <queries/query_support.hpp>
#include <string>
#include <vector>
#include <wx_pc.hpp>

#include "bwt.hpp"
#include "util/timer.hpp"
#include "wt_naive.hpp"

namespace alx {
template <typename t_word = size_t>
class r_index {
 public:
  r_index() = default;

  r_index(std::string const& text) {
    std::cout << "DETAILS r-index";
    benchutil::timer timer;
    alx::bwt input_bwt(text);
    std::cout << " time_bwt=" << timer.get_and_reset();
    build_structure(input_bwt);
  }

  r_index(alx::bwt const& bwt) : m_text_size(bwt.size()), m_bwt_primary_index(bwt.primary_index) {
    build_structure(bwt);
  }

  size_t occ(std::string const& pattern, bool debug = false) {
    
    unsigned char c = pattern.back();
    size_t span_start = m_char_sum[c];
    size_t span_end = m_char_sum[c + 1];
    if(debug) {
      //for(auto const& a : m_run_lengths) {
      for(size_t i = 0; i < m_run_lengths.size(); ++i) {
        auto const& a = m_run_lengths[i];
        if(a.size() > 1) {
          std::cout << i << ": " << a << std::endl;
        }
      }
      std::cout << "\nIteration " << pattern.size()-1 << " after scanning " << c << std::endl;
      std::cout << "Span[" << span_start << ", " << span_end << "]" << std::endl;
    }

    for (size_t i = pattern.size() - 1; i != 0; --i) {
      c = pattern[i - 1];
      size_t rank_start = rank(span_start - (span_start <= m_bwt_primary_index ? 0 : 1), c, debug);
      size_t rank_end = rank(span_end - (span_end <= m_bwt_primary_index ? 0 : 1), c, debug);
      span_start = m_char_sum[c] + rank_start;
      span_end = m_char_sum[c] + rank_end;
      if(debug) {
        std::cout << "Iteration " << i-1 << " after scanning " << c << std::endl;
        std::cout << "Rank[" << rank_start << ", " << rank_end << "]" << std::endl;
        std::cout << "Span[" << span_start << ", " << span_end << "]" << std::endl;
      }
      if (span_start == span_end) {
        return 0;
      }
      
    }
    return span_end - span_start;
  }

  size_t num_runs() {
    return m_run_starts.size();
  }

  std::vector<size_t> locate_all(std::string const& pattern) {
    return std::vector<size_t>(pattern.length());
  }

  r_index load_from_file(std::string path) { return r_index{}; }
  void save_to_file(std::string path) {}

 private:
  size_t m_text_size;
  size_t m_bwt_primary_index;
  std::vector<size_t> m_run_starts;
  std::vector<unsigned char> m_run_letters;
  wavelet_tree m_run_letters_wt;
  std::array<std::vector<size_t>, 256> m_run_lengths;
  std::array<size_t, 257> m_char_sum;

  size_t pred(size_t pos) {
    return 0;
  }

  size_t rank(size_t pos, char c, bool debug = false) {
    //if(pos >= m_bwt_primary_index)
    size_t kth_run = std::distance(m_run_starts.begin(), std::upper_bound(m_run_starts.begin(), m_run_starts.end(), pos));  //TODO: pred(pos)
    kth_run--; //Because we got the first greater position 
    size_t run_start = m_run_starts[kth_run];
    size_t num_c_run = (kth_run != 0) ? m_run_letters_wt.rank(kth_run-1, c) : 0;

    unsigned char run_symbol = m_run_letters[kth_run];
    if(debug) {
      std::cout << "rank_" << c << "(" << pos << ") = " 
      << m_run_lengths[c][num_c_run] + (c == run_symbol ? (pos - run_start + 1) : 0) << std::endl;
      std::cout << "kth_run=" << kth_run << " run_start=" << run_start << " num_c_run=" << num_c_run << std::endl;
    }

    return m_run_lengths[c][num_c_run] + (c == run_symbol ? (pos - run_start + 1) : 0);
  }

  void build_structure(alx::bwt const& input_bwt) {
    std::cout << "DETAILS r-index";
    benchutil::timer timer;
    // Prepare m_run_length
    for (size_t i = 0; i < 256; ++i) {
      m_run_lengths[i].push_back(0);
    }
    // Build data structure
    for (size_t run_start = 0; run_start < input_bwt.size();) {
      unsigned char run_letter = input_bwt.last_row[run_start];
      m_run_starts.push_back(run_start);
      m_run_letters.push_back(run_letter);

      size_t run_end = run_start + 1;
      while (run_end < input_bwt.size() && input_bwt.last_row[run_end] == run_letter) {
        ++run_end;
      }
      m_run_lengths[run_letter].push_back(m_run_lengths[run_letter].back() + run_end - run_start);
      run_start = run_end;
    }

    // Build wavelet tree
    m_run_letters_wt = wavelet_tree(m_run_letters);

    // Build m_char_sum
    for(size_t i = 0; i < m_char_sum.size(); ++i) {
      m_char_sum[i] = 0;
    }
    for (auto c : input_bwt.last_row) {
      ++m_char_sum[c];
    }
    std::exclusive_scan(m_char_sum.begin(), m_char_sum.end(), m_char_sum.begin(), 0);

    std::cout << " num_runs=" << m_run_starts.size();
    std::cout << " time_index=" << timer.get() << '\n';
    // std::cout << "Runs   " << m_run_starts << "\n";
    // std::cout << "Run L: " << m_run_letters << "\n";
  }
};
}  // namespace alx
