#pragma once

#include <array>
#include <queries/query_support.hpp>
#include <string>
#include <tdc/pred/index.hpp>
#include <util/type_for_bytes.hpp>
#include <vector>
#include <wx_pc.hpp>

#include "bwt.hpp"
#include "util/spacer.hpp"
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
    if (debug) {
      /*
      //for(auto const& a : m_run_lengths) {
      for(size_t i = 0; i < m_run_lengths.size(); ++i) {
        auto const& a = m_run_lengths[i];
        if(a.size() > 1) {
          std::cout << i << ": " << a << std::endl;
        }
      }*/
      std::cout << "\nIteration " << pattern.size() - 1 << " after scanning " << c << std::endl;
      std::cout << "Span[" << span_start << ", " << span_end << "]" << std::endl;
    }

    for (size_t i = pattern.size() - 1; i != 0; --i) {
      c = pattern[i - 1];
      size_t rank_start = rank(span_start - (span_start <= m_bwt_primary_index ? 0 : 1), c, false);
      size_t rank_end = rank(span_end - (span_end <= m_bwt_primary_index ? 0 : 1), c, false);
      span_start = m_char_sum[c] + rank_start;
      span_end = m_char_sum[c] + rank_end;
      if (debug) {
        std::cout << "Iteration " << i - 1 << " after scanning " << c << std::endl;
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
  std::vector<size_t> m_run_starts;          // 8r B
  tdc::pred::Index m_pred;                   // small enough
  std::vector<unsigned char> m_run_letters;  // 1r B
  //wavelet_tree m_run_letters_wt;             // 1r B
  std::unique_ptr<wavelet_structure> m_run_letters_wt;
  std::unique_ptr<query_support<>> m_run_letters_rank;

  std::array<std::vector<size_t>, 256> m_run_lengths;  // 8r B
  std::array<size_t, 257> m_char_sum;                  // 8s B

  size_t pred(size_t i) {
    return m_pred.predecessor(m_run_starts.data(), m_run_starts.size(), i).pos;
  }

  size_t run_rank(unsigned char c, size_t i) {
    return m_run_letters_rank->rank(c, i+1);
  }

  /*
  size_t run_rank_old(unsigned char c; size_t i) {
    return m_run_letters_wt.rank(i, c) : 0;
  }
  */

  size_t rank(size_t pos, unsigned char c, bool debug = false) {
    // size_t kth_run = std::distance(m_run_starts.begin(), std::upper_bound(m_run_starts.begin(), m_run_starts.end(), pos));  // TODO: pred(pos)
    // kth_run--;                                                                                                              // Because we got the first greater position
    size_t kth_run = pred(pos);

    size_t run_start = m_run_starts[kth_run];
    //size_t num_c_run = (kth_run != 0) ? m_run_letters_wt.rank(kth_run - 1, c) : 0;
    size_t num_c_run = run_rank(c, kth_run-1);

    unsigned char run_symbol = m_run_letters[kth_run];
    if (debug) {
      std::cout << "rank_" << c << "(" << pos << ") = "
                << m_run_lengths[c][num_c_run] + (c == run_symbol ? (pos - run_start + 1) : 0) << std::endl;
      std::cout << "kth_run=" << kth_run << " run_start=" << run_start << " num_c_run=" << num_c_run << std::endl;
    }

    return m_run_lengths[c][num_c_run] + (c == run_symbol ? (pos - run_start + 1) : 0);
  }

  void build_structure(alx::bwt const& input_bwt) {
    benchutil::timer timer;
    benchutil::spacer spacer;
    {
      benchutil::timer timer;
      benchutil::spacer spacer;
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
      std::cout << " runs_time=" << timer.get_and_reset()
                << " runs_mem_peak=" << spacer.get_peak()
                << " runs_mem_ds=" << spacer.get()
                << '\n';
    }

    // Build predecessor data structure
    {
      benchutil::timer timer;
      benchutil::spacer spacer;

      m_pred = tdc::pred::Index(m_run_starts.data(), m_run_starts.size(), 7);
      verify_pred();

      std::cout << " pred_time=" << timer.get_and_reset()
                << " pred_mem_peak=" << spacer.get_peak()
                << " pred_mem_ds=" << spacer.get()
                << '\n';
    }

    // Build wavelet tree
    {
      benchutil::timer timer;
      benchutil::spacer spacer;

      //m_run_letters_wt = wavelet_tree(m_run_letters);
      m_run_letters_wt = std::make_unique<wavelet_structure>(wx_pc<uint8_t, true>::compute(m_run_letters.data(), m_run_letters.size(), 8));
      m_run_letters_rank = std::make_unique<query_support<>>(*m_run_letters_wt);

      std::cout << " wt_time=" << timer.get_and_reset()
                << " wt_mem_peak=" << spacer.get_peak()
                << " wt_mem_ds=" << spacer.get()
                << '\n';
    }

    // Build m_char_sum
    {
      benchutil::timer timer;
      benchutil::spacer spacer;

      for (size_t i = 0; i < m_char_sum.size(); ++i) {
        m_char_sum[i] = 0;
      }
      for (auto c : input_bwt.last_row) {
        ++m_char_sum[c];
      }
      std::exclusive_scan(m_char_sum.begin(), m_char_sum.end(), m_char_sum.begin(), 0);

      std::cout << " carray_time=" << timer.get_and_reset()
                << " carray_mem_peak=" << spacer.get_peak()
                << " carray_mem_ds=" << spacer.get()
                << '\n';
    }

    std::cout << " num_runs=" << m_run_starts.size();
    std::cout << " index_time=" << timer.get_and_reset()
              << " index_mem_peak=" << spacer.get_peak()
              << " index_mem_ds=" << spacer.get()
              << '\n';
  }

  bool verify_pred() {
    // Careful: For external memory (m_run_starts.size() == 0) is possible.
    // if (m_run_starts.size() == 0) {return true;}

    if (m_run_starts[0] != 0) {
      std::cerr << "\nFirst entry in m_runs_starts should be 0!" << std::endl;
      return false;
    }
    for (size_t i = 1; i < m_run_starts.size(); ++i) {
      for (size_t j = m_run_starts[i - 1]; j < m_run_starts[i]; ++j) {
        if (pred(j) != i - 1) {
          std::cout << "\nPredecessor of " << j << ": " << pred(j) << " should be " << i - 1 << std::endl;
          return false;
        }
      }
      for (size_t j = m_run_starts.back(); j < m_text_size; ++j) {
        if (pred(j) != m_run_starts.size() - 1) {
          std::cout << "\nPredecessor of " << j << ": " << pred(j) << " should be " << m_run_starts.size() - 1 << std::endl;
          return false;
        }
      }
    }
    std::cout << " verify_predecessor=true";
    return true;
  }

  bool verify_wt() {
    std::vector histogram(256, 0);

    for (size_t i = 0; i < m_text_size; ++i) {
      ++histogram[m_run_letters[i]];
      for (size_t c = 0; c < 256; ++c) {
        if (run_rank(c, i) != histogram[c]) {
          std::cout << "Rank_sigma wrong at i: " << i << " c:" << c << '\n';
          return false;
        }
      }
    }
    std::cout << " verify_rank=true";
    return true;
  }
};
}  // namespace alx
