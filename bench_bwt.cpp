#include <divsufsort64.h>
#include <libsais64.h>

#include "util/io.hpp"
#include "util/spacer.hpp"
#include "util/timer.hpp"

struct bwt_bench {
  std::string const path = "/home/herlez/text/";
  std::string text_path;

  size_t print_first_bwt_chars = 20;
  void run() {
    // Load Text
    std::string text = alx::io::load_text(text_path);
    std::cout << "T:  " << text.substr(0, print_first_bwt_chars) << "\n";
    alx::bwt bwt;
    bwt.last_row.resize(text.size());

    // Build BWT
    alx::benchutil::spacer spacer;
    alx::benchutil::timer timer;

    spacer.reset();
    timer.reset();
    {
      std::vector<int64_t> sa(text.size() + 100);
      bwt.primary_index = libsais64_bwt(reinterpret_cast<const uint8_t*>(text.data()), reinterpret_cast<uint8_t*>(bwt.last_row.data()), sa.data(), text.size(), int64_t{101}, nullptr);
    }
    std::cout << "RESULT algo=libsais time=" << timer.get() << " mempeak=" << spacer.get_peak() << '\n';
    std::cout << "L:  " << bwt.last_row.substr(0, print_first_bwt_chars) << " (" << bwt.primary_index << ")\n";
    {
      std::string new_text(text.size(), ' ');
      std::vector<int64_t> sa(text.size() + 1);
      libsais64_unbwt(reinterpret_cast<const uint8_t*>(bwt.last_row.data()), reinterpret_cast<uint8_t*>(new_text.data()), sa.data(), new_text.size(), nullptr, bwt.primary_index);
      std::cout << "T': " << new_text.substr(0, print_first_bwt_chars) << "\n";
    }

    spacer.reset();
    timer.reset();
    {
      std::vector<int64_t> sa(text.size() + 100);
      bwt.primary_index = divbwt64(reinterpret_cast<const unsigned char*>(text.data()), reinterpret_cast<unsigned char*>(bwt.last_row.data()), sa.data(), text.size());
    }
    std::cout << "RESULT algo=divsufsort time=" << timer.get() << " mempeak=" << spacer.get_peak() << '\n';
    std::cout << "L:  " << bwt.last_row.substr(0, print_first_bwt_chars) << " (" << bwt.primary_index << ")\n";
    // Verify BWT
  }
};

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Format: bwt text_path\n";
    return 0;
  }

  bwt_bench benchmark;
  benchmark.text_path = argv[1];

  benchmark.run();
}