#include <cmdline_parser.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "bwt.hpp"
#include "r_index_alx.hpp"
#include "r_index_prezza.hpp"
#include "util/io.hpp"
#include "util/spacer.hpp"
#include "util/timer.hpp"

enum benchmark_mode { from_text,
                      from_bwt,
                      from_index };

class index_benchmark {
 public:
  std::string algo;

  benchmark_mode mode = benchmark_mode::from_text;
  std::filesystem::path input_path;
  std::filesystem::path patterns_path;
  size_t num_patterns{std::numeric_limits<size_t>::max()};

 public:
  template <template <typename> typename t_index>
  void run(std::string const& algo) {
    // Load text
    std::string text_name = input_path.filename();
    std::string text = alx::io::load_text(input_path);
  
    if constexpr(std::is_same_v<decltype(t_index()), decltype(ri::r_index())>) {
      assert(text.back() == '\00');
      text.pop_back(); //prezza index can't handle terminal
    }

    // Load queries
    std::vector<std::string> patterns;
    {
      alx::benchutil::spacer spacer;
      alx::benchutil::timer timer;

      patterns = alx::io::load_patterns(patterns_path, num_patterns);
      assert(patterns.size() <= num_patterns);

      std::cout << "patterns_load_time=" << timer.get()
                << " mem=" << spacer.get()
                << " patterns_path=" << patterns_path
                << " patterns_num=" << patterns.size();
      if (patterns.size() != 0) {
        std::cout << " patterns_len=" << patterns.front().size() << "\n";
      }

      for(auto const& p : patterns) {
        if(p.find('\00') != p.npos) {
          std::cout << "Pattern contains terminal!\n";
          return;
        }
      }
    }

    // Build index
    t_index r_index;

    std::cout << "RESULT"
              << " algo=" << algo
              << " mode=" << mode
              << " text=" << text_name
              << " size=" << text.size();
    alx::benchutil::timer timer;
    alx::benchutil::spacer spacer;
    if (mode == benchmark_mode::from_text) {
      r_index = t_index(text);
    } else if (mode == benchmark_mode::from_bwt) {
      if constexpr (std::is_same_v<decltype(t_index()), decltype(alx::r_index())>) {
        std::filesystem::path last_row_path = input_path;
        last_row_path += ".bwt";
        std::filesystem::path primary_index_path = input_path;
        primary_index_path += ".prm";

        alx::bwt bwt(last_row_path, primary_index_path);
        r_index = t_index(bwt);
      }
    } else if (mode == benchmark_mode::from_index) {
      //std::filesystem::path index_path = input_path;
      //index_path += (algo == "prezza") ? ".ri" : ".rix";
      //r_index = t_index(index_path);
    }

    std::cout << " ds_time=" << timer.get()
              << " ds_mem=" << spacer.get()
              << " ds_mempeak=" << spacer.get_peak();

    // Counting Queries
    std::vector<size_t> count_results;
    timer.reset();
    for (std::string& q : patterns) {
      count_results.push_back(r_index.occ(q));
    }
    // for (auto i: count_results)
    // std::cout << i << ' ';

    std::cout << " c_time=" << timer.get()
              << " c_sum=" << accumulate(count_results.begin(), count_results.end(), 0)
              << "\n";

    timer.reset();

    /*
    // Enumerating Queries
    std::vector<sdsl::tVI> enumerate_results;
    for(std::string const& q : count_queries) {
      enumerate_results.push_back(r_index.locate_all(q)); //.size()
    }
    std::cout << "e_time=" << timer.get();
    */

    /*
        //Check
        size_t num_errors = 0;
        size_t matches = 0;
        assert(count_results.size() == enumerate_results.size());
        for (size_t i = 0; i < count_results.size(); ++i) {
          matches += count_results[i];
          if (count_results[i] != enumerate_results[i].size()) {
            ++num_errors;
          }
        }
    */
  }
};

int main(int argc, char* argv[]) {
  index_benchmark benchmark;

  tlx::CmdlineParser cp;
  cp.set_description("Benchmark for text indices");
  cp.set_author("Alexander Herlez <alexander.herlez@tu-dortmund.de>\n");

  std::string input_path;
  cp.add_param_string("input_path", input_path, "Path to the input text/BWT/index without file suffix. Enter \"~/text/english\" to load \"~/text/english.bwt\"");

  unsigned int mode;
  cp.add_param_uint("mode", mode, "Mode: [0]:Text->Index [1]:BWT->Index [2]:Load Index from file");

  std::string patterns_path;
  cp.add_param_string("patterns_path", patterns_path, "Path to pizza&chili patterns");

  cp.add_size_t('q', "num_patterns", benchmark.num_patterns,
                "Number of queries (default=all)");

  if (!cp.process(argc, argv)) {
    std::exit(EXIT_FAILURE);
  }
  benchmark.input_path = input_path;
  benchmark.patterns_path = patterns_path;
  benchmark.mode = static_cast<benchmark_mode>(mode);

  std::cout << "r-index herlez------------------------------------------------------------------\n";
  benchmark.run<alx::r_index>("herlez");
  std::cout << "r-index prezza------------------------------------------------------------------\n";
  benchmark.run<ri::r_index>("prezza");
}
