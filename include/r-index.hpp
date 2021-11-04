#pragma once

#include <array>
#include <string>
#include <vector>

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

    std::cout << " num_runs=" << m_run_starts.size();
    std::cout << " time_index=" << timer.get() << '\n';
    //std::cout << "Runs   " << m_run_starts << "\n";
    //std::cout << "Run L: " << m_run_letters << "\n";
  }
};
}  // namespace alx
