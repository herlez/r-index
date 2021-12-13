#include <divsufsort64.h>
#include <libsais64.h>

#include "util/io.hpp"
#include "util/spacer.hpp"
#include "util/timer.hpp"

struct bwt_bench {
  std::filesystem::path text_path;

  void run() {
    // Load Text
    std::string text = alx::io::load_text(text_path);

    alx::bwt bwt;

    // Build BWT
    alx::benchutil::spacer spacer;
    alx::benchutil::timer timer;

    spacer.reset();
    timer.reset();
    {
      bwt.last_row.resize(text.size());
      std::vector<int64_t> sa(text.size() + 100);
      bwt.primary_index = libsais64_bwt(reinterpret_cast<const uint8_t*>(text.data()), reinterpret_cast<uint8_t*>(bwt.last_row.data()), sa.data(), text.size(), int64_t{101}, nullptr);
    }
    std::cout << "RESULT algo=libsais_100 ds_time=" << timer.get() << " ds_mempeak=" << spacer.get_peak();
    {
      std::string new_text(text.size(), ' ');
      std::vector<int64_t> sa(text.size() + 1);
      libsais64_unbwt(reinterpret_cast<const uint8_t*>(bwt.last_row.data()), reinterpret_cast<uint8_t*>(new_text.data()), sa.data(), new_text.size(), nullptr, bwt.primary_index);
      std::cout << " correct=" << (new_text == text) << '\n';
    }

    spacer.reset();
    timer.reset();
    {
      bwt.last_row.resize(text.size());
      std::vector<int64_t> sa(text.size() + 100);
      bwt.primary_index = divbwt64(reinterpret_cast<const unsigned char*>(text.data()), reinterpret_cast<unsigned char*>(bwt.last_row.data()), sa.data(), text.size());
    }
    std::cout << "RESULT algo=divsufsort_100 time=" << timer.get() << " mempeak=" << spacer.get_peak();
    {
      std::string new_text(text.size(), ' ');
      std::vector<int64_t> sa(text.size() + 1);
      inverse_bw_transform64(reinterpret_cast<const uint8_t*>(bwt.last_row.data()), reinterpret_cast<uint8_t*>(new_text.data()), sa.data(), new_text.size(), bwt.primary_index);
      std::cout << " correct=" << (new_text == text) << '\n';
    }
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