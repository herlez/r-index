#include <array>
#include <filesystem>
#include <tdc/uint/uint40.hpp>
#include <vector>
#include <iostream>
#include <string>

#include "util/io.hpp"
#include "util/spacer.hpp"
#include "util/timer.hpp"

struct gen_stats {
  std::filesystem::path text_path;

  void run() {
    // Load Text
    std::string text = alx::io::load_text(text_path);
    std::string text_name = text_path.filename();
    std::cout << "RESULT"
              << " algo="
              << "gen_stats"
              << " text=" << text_name
              << " alphabet=" << alx::io::alphabet_size(text)
              << " size=" << text.size();

    std::vector<tdc::uint40_t> run_starts;
    std::vector<unsigned char> run_letters;
    std::array<std::vector<tdc::uint40_t>, 256> run_lengths;

    // Generate Stats
    alx::benchutil::spacer spacer;
    alx::benchutil::timer timer;

    spacer.reset();
    timer.reset();

    // Prepare m_run_length
    for (size_t i = 0; i < 256; ++i) {
      run_lengths[i].push_back(0);
    }

    // Build data structure
    for (size_t run_start = 0; run_start < text.size();) {
      unsigned char run_letter = text[run_start];
      run_starts.push_back(run_start);
      run_letters.push_back(run_letter);

      size_t run_end = run_start + 1;
      while (run_end < text.size() && text[run_end] == run_letter) {
        ++run_end;
      }
      run_lengths[run_letter].push_back(run_lengths[run_letter].back() + run_end - run_start);
      run_start = run_end;
    }
    run_starts.shrink_to_fit();
    for (auto& a : run_lengths) {
      a.shrink_to_fit();
    }
    std::cout << " runs_time=" << timer.get_and_reset()
              << " runs_mem=" << spacer.get()
              << " runs_mempeak=" << spacer.get_peak()
              << " run_num=" << run_letters.size();
  std::cout << '\n';
  }
};

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Format: gen_stats text_path\n";
    return 0;
  }

  gen_stats generator;
  generator.text_path = argv[1];

  generator.run();
}