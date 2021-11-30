#include <wavelet_tree/wavelet_tree.hpp>

#include "bwt.hpp"
#include "util/io.hpp"
#include "util/spacer.hpp"
#include "util/timer.hpp"
#include "wt_naive.hpp"

struct wt_bench {
  std::string const path = "/home/herlez/text/";
  std::string bwt_path;

  size_t print_first_bwt_chars = 20;
  void run() {
    {
      // Load BWT
      std::string bwt = alx::bwt::load_from_file(bwt_path).last_row;
      // std::cout << "T:  " << bwt.substr(0, print_first_bwt_chars) << "\n";

      // Build WT
      alx::benchutil::spacer spacer;
      alx::benchutil::timer timer;

      spacer.reset();
      timer.reset();

      alx::wavelet_tree wt(bwt);

      std::cout << "RESULT algo=alx_wt construct_time=" << timer.get()
                << " mempeak=" << spacer.get_peak()
                << " mem_ds=" << spacer.get();

      // Benchmark queries
      size_t num_queries = 10'000;
      timer.reset();
      std::vector<size_t> query_results;
      const size_t step = 2;
      for (size_t i = 0; i < std::min(bwt.size() / step, num_queries); ++i) {
        query_results.push_back(wt.rank(i * step, bwt[i * step / 2]));
      }
      std::cout << " rank_time=" << timer.get()
                << " num_queries=" << query_results.size();

      // Verify Access
      size_t errors = 0;
      timer.reset();
      for (size_t i = 0; i < std::min(bwt.size(), num_queries); ++i) {
        auto cor_char = bwt[i];
        if (wt[i] != cor_char) {
          ++errors;
        }
      }

      std::cout << " errors_access=" << errors
                << " ver_access=" << timer.get();

      // Verify Rank
      errors = 0;
      timer.reset();
      for (size_t i = 0; i < query_results.size(); ++i) {
        size_t cor_result = std::count(bwt.begin(), bwt.begin() + i * step + 1, bwt[i * step / 2]);
        if (query_results[i] != cor_result) {
          std::cout << "\n\nERROR for rank(" << i * step << ", " << bwt[i * step / 2] << ") cor=" << cor_result << " wrong=" << query_results[i] << "\n";
          wt.rank(i * step, bwt[i * step / 2], true);
          return;
          ++errors;
        }
      }

      std::cout << " errors_rank=" << errors
                << " ver_rank=" << timer.get()
                << " text_path=" << bwt_path
                << " text_size=" << bwt.size();

      std::cout << '\n';
    }

    {
      // Load BWT
      std::string bwt = alx::bwt::load_from_file(bwt_path).last_row;
      std::vector<uint8_t> text(bwt.begin(), bwt.end());
      // std::cout << "T:  " << bwt.substr(0, print_first_bwt_chars) << "\n";

      // Build WT
      alx::benchutil::spacer spacer;
      alx::benchutil::timer timer;

      spacer.reset();
      timer.reset();

      auto wm = pasta::make_wm<pasta::BitVector>(text.begin(), text.end(), 256);

      std::cout << "RESULT algo=pasta_wm construct_time=" << timer.get()
                << " mempeak=" << spacer.get_peak()
                << " mem_ds=" << spacer.get();

      // Benchmark queries
      size_t num_queries = 10'000;
      timer.reset();
      std::vector<size_t> query_results;
      const size_t step = 2;
      for (size_t i = 0; i < std::min(bwt.size() / step, num_queries); ++i) {
        query_results.push_back(wm.rank(i * step+1, bwt[i * step / 2]));
      }
      std::cout << " rank_time=" << timer.get()
                << " num_queries=" << query_results.size();

      // Verify Access
      size_t errors = 0;
      timer.reset();
      for (size_t i = 0; i < std::min(bwt.size(), num_queries); ++i) {
        auto cor_char = bwt[i];
        if (wm[i] != cor_char) {
          ++errors;
        }
      }

      std::cout << " errors_access=" << errors
                << " ver_access=" << timer.get();

      // Verify Rank
      errors = 0;
      timer.reset();
      for (size_t i = 0; i < query_results.size(); ++i) {
        size_t cor_result = std::count(bwt.begin(), bwt.begin() + i * step + 1, bwt[i * step / 2]);
        if (query_results[i] != cor_result) {
          std::cout << "\n\nERROR for rank(" << i * step << ", " << bwt[i * step / 2] << ") cor=" << cor_result << " wrong=" << query_results[i] << "\n";
          return;
          ++errors;
        }
      }

      std::cout << " errors_rank=" << errors
                << " ver_rank=" << timer.get()
                << " text_path=" << bwt_path
                << " text_size=" << bwt.size();

      std::cout << '\n';
    }
  }
};

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Format: wt text_path\n";
    return 0;
  }

  wt_bench benchmark;
  benchmark.bwt_path = argv[1];

  benchmark.run();
}