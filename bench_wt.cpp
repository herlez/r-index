#include <array>
#include <wavelet_tree/wavelet_tree.hpp>

#include "util/io.hpp"
#include "util/spacer.hpp"
#include "util/timer.hpp"

enum benchmark_algorithm { pasta_wt };

constexpr const char* algo_name(benchmark_algorithm algo) {
  switch (algo) {
    case benchmark_algorithm::pasta_wt:
      return "pasta_wt";
  }
  return "UNKNOWN";
}

struct wt_bench {
  std::filesystem::path text_path;
  benchmark_algorithm algo;

  void run() {
    // Load text
    std::string text_name = text_path.filename();
    std::string text = alx::io::load_text(text_path);

    std::cout << "RESULT"
              << " algo=" << algo_name(algo)
              << " text=" << text_name
              << " alphabet=" << alx::io::alphabet_size(text)
              << " size=" << text.size();

    // Generate queries
    std::vector<std::pair<size_t, unsigned char>> rank_queries;
    size_t num_queries = 1'000'000;
    size_t step = 2;
    for (size_t i{0}; i < num_queries && (num_queries * step < text.size()); ++i) {
      rank_queries.push_back({i * step, text[(i * step) / 2]});
    }
    std::vector<size_t> rank_results(rank_queries.size());

   if (algo == benchmark_algorithm::pasta_wt) {
      // Build pasta_wt
      alx::benchutil::spacer spacer;
      alx::benchutil::timer timer;

      auto wt = pasta::make_wm<pasta::BitVector>(reinterpret_cast<uint8_t*>(text.data()), reinterpret_cast<uint8_t*>(text.data() + text.size()), alx::io::alphabet_size(text));

      std::cout << " ds_time=" << timer.get()
                << " ds_mem=" << spacer.get()
                << " ds_mempeak=" << spacer.get_peak();

      timer.reset();
      for (size_t i{0}; i < rank_queries.size(); ++i) {
        std::pair<size_t, unsigned char> q = rank_queries[i];
        rank_results[i] = wt.rank(q.first, q.second);
      }
      std::cout << " r_time=" << timer.get()
                << " r_sum=" << accumulate(rank_results.begin(), rank_results.end(), 0);
    }

    // Verify rank queries
    std::array<size_t, 256> histogram;
    histogram.fill(0);
    size_t filled_up_to = 0;
    size_t errors = 0;
    for(size_t i = 0; i < rank_queries.size(); ++i) {
      size_t next_i = rank_queries[i].first;
      unsigned char next_char = rank_queries[i].second;

      while(filled_up_to <= next_i + (algo == benchmark_algorithm::pasta_wt)) {
        ++histogram[text[filled_up_to]];
        ++filled_up_to;
      }
      if(rank_results[i] != histogram[next_char]) {
        ++errors;
      }
    }

    std::cout << " errors=" << errors << '\n';
  }
};

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Format: wt text_path algo_number\n";
    return 0;
  }

  wt_bench benchmark;
  benchmark.text_path = argv[1];
  benchmark.algo = static_cast<benchmark_algorithm>(std::atoi(argv[2]));

  benchmark.run();
}