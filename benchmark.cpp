#include <cmdline_parser.hpp>
#include <filesystem>
#include <iostream>
#include <r_index.hpp>
#include <string>
#include <vector>

#include "r-index.hpp"
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
  std::filesystem::path text_path;
  std::filesystem::path bwt_path;
  std::filesystem::path index_path;

  std::filesystem::path queries_path;

  unsigned int num_runs = 1;

 public:
  template <template <typename> typename t_index>
  void run(std::string const& algo) {
    // Load text
    std::string text_name = text_path.filename();
    std::string text = alx::io::load_text(text_path);

    // Generate queries
    std::vector<std::string> count_queries = alx::io::generate_queries(text, size_t{1} << 20, 10);
    std::vector<size_t> count_results;

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
      bwt_path = text_path;
      bwt_path += ".bwt";
      alx::bwt bwt(bwt_path);
      r_index = t_index(bwt);
    } else if (mode == benchmark_mode::from_index) {
      index_path = text_path;
      index_path += (algo == "prezza") ? ".ri" : ".rix";
      r_index = t_index(index_path);
    }

    std::cout << " ds_time" << timer.get()
              << " ds_mem=" << spacer.get()
              << " ds_mempeak=" << spacer.get_peak();

  
    // Counting Queries
    timer.reset();
    for (std::string const& q : count_queries) {
      count_results.push_back(r_index.occ(q));
    }
    std::cout << " c_time=" << timer.get()
              << " c_sum=" << accumulate(count_results.begin(), count_results.end(), 0);

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

  std::string text_path;
  cp.add_param_string("text", text_path, "Path to the input text");

  unsigned int mode;
  cp.add_param_uint("mode", mode, "Mode: [0]:Text->Index [1]:BWT->Index [2]:Load Index from file");

  // cp.add_param_string("queries", benchmark.queries_path, "Path where queries");

  // cp.add_string('a', "algorithm", benchmark.algo, "Name of index");

  // cp.add_flag('c', "check", benchmark.check, "Check enumerated positions for pattern");
  cp.add_uint('r', "runs", benchmark.num_runs,
              "Number of runs that are used to "
              "report an average running time (default=5).");

  if (!cp.process(argc, argv)) {
    std::exit(EXIT_FAILURE);
  }
  benchmark.text_path = text_path;
  benchmark.mode = static_cast<benchmark_mode>(mode);

  std::cout << "r-index herlez------------------------------------------------------------------\n";
  benchmark.run<alx::r_index>("herlez");
  // std::cout << "r-index prezza------------------------------------------------------------------\n";
  // benchmark.run<ri::r_index>("prezza");
}
