#pragma once

#include <array>
#include <queries/query_support.hpp>
#include <string>
#include <vector>
#include <wx_pc.hpp>

#include "bwt.hpp"
#include "util/timer.hpp"

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

  r_index(alx::bwt const& bwt) {
    build_structure(bwt);
  }

  size_t occ(std::string const& pattern) {
    return pattern.length();
  }

  std::vector<size_t> locate_all(std::string const& pattern) {
    return std::vector<size_t>(pattern.length());
  }

  r_index load_from_file(std::string path) { return r_index{}; }
  void save_to_file(std::string path) {}

 private:
  std::vector<size_t> m_run_starts;
  std::vector<unsigned char> m_run_letters;
  std::array<std::vector<size_t>, 256> m_run_lengths;
  //Dummy wavelet matrix, because default constructor is private
  wavelet_structure m_wm = wx_pc<uint8_t, false>::compute(m_run_letters.data(), 0, 0);
  query_support m_qs;
  

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

    // auto max_char = reduce_alphabet(input_bwt.last_row);
    // uint64_t levels = levels_for_max_char(max_char);
    auto vec = std::vector<uint8_t>(input_bwt.last_row.begin(), input_bwt.last_row.end());
    size_t levels = 8;

    //wavelet_structure m_wm = wx_pc<uint8_t, false>::compute(vec.data(), vec.size(), levels);
    m_wm = wx_pc<uint8_t, false>::compute(vec.data(), vec.size(), levels);
    //std::unique_ptr<wavelet_structure> m_wm = std::unique_ptr<wavelet_structure>(ws);
    //m_wm = 
    std::vector<size_t> symbol_counts(256, 0);
    //std::unique_ptr<query_support<>> = query_support(m_wm);
    
  

    std::cout << " num_runs=" << m_run_starts.size();
    std::cout << " time_index=" << timer.get() << '\n';
    // std::cout << "Runs   " << m_run_starts << "\n";
    // std::cout << "Run L: " << m_run_letters << "\n";
  }
};
}  // namespace alx
