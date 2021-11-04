#include <cmdline_parser.hpp>
#include <filesystem>
#include <iostream>
#include <r_index.hpp>
#include <string>
#include <vector>

#include "r-index.hpp"
#include "util/io.hpp"
#include "util/timer.hpp"

class index_benchmark {
 public:
  std::string const path = "/home/herlez/text/";
  std::string algo;

  std::string text_path;
  std::string bwt_path;
  std::string index_path;
  std::string queries_path;

  bool check;
  unsigned int num_runs;

 public:
  template <template <typename> typename t_index>
  void run(std::string const& algo) {
    // Either
    constexpr bool index_herlez = std::is_same<alx::r_index<uint64_t>, t_index<uint64_t>>::value;

    // Load text
    std::string text_name = alx::io::get_text_name(text_path);
    std::string text = alx::io::load_text(text_path);

    // Load bwt
    bwt_path = text_path + ".bwt";
    alx::bwt bwt;
    if (index_herlez) {
      bwt = alx::io::load_bwt(bwt_path, text);
      if (bwt.to_text() != text) {
        std::cout << "BWT WRONG\n";
        std::cout << "T:  " << text.substr(0, 30) << "\n"
                  << "L:  " << bwt.last_row.substr(0, 30) << " (" << bwt.primary_index << ")\n"
                  << "T': " << bwt.to_text().substr(0, 30) << "\n";
      }
    }

    // Load Queries
    std::vector<std::string> queries = alx::io::generate_queries(text, size_t{1} << 8, 20);
    // queries_path = text_path + "queries/" + text_name;
    // std::vector<std::string> queries = load_queries(queries_path);

    {
      // Build r-index
      std::string index_path = path + "index/" + text_name;
      std::string index_suffix = (algo == "prezza" ? ".ri" : ".rix");
      t_index r_index;

      alx::benchutil::timer timer;

      if (std::filesystem::exists(index_path + index_suffix)) {
        r_index.load_from_file(index_path + index_suffix);
        std::cout << "#READ index from=" << index_path + index_suffix << " time=" << timer.get() << '\n';
      } else {
        if constexpr (index_herlez) {
          // std::cout << bwt.last_row.substr(0, 30) << "\n";
          // std::cout << bwt.to_text().substr(0, 30) << "\n";
          r_index = t_index(bwt);
        } else {
          r_index = t_index(text);
        }
        std::cout << "RESULT algo=" << algo
                  << " build_time=" << timer.get_and_reset()
                  << '\n';
        r_index.save_to_file(index_path);
        std::cout << "#WRITE index to=" << index_path + index_suffix << " time=" << timer.get_and_reset() << '\n';
      }

      // Counting queries
      std::cout << "RESULT algo=" << algo;
      std::vector<ulint> count_results;
      timer.reset();
      for (std::string& q : queries) {
        count_results.push_back(r_index.occ(q));
      }
      std::cout << " count_time=" << timer.get()
                << " count_sum=" << accumulate(count_results.begin(), count_results.end(), 0);

      // Enumerate queries
      /*
      std::vector<sdsl::tVI> enumerate_results;
      timer.reset();
      for (std::string& q : queries) {
        enumerate_results.push_back(r_index.locate_all(q));  //.size()
      }
      std::cout << " enumerate_time=" << timer.get();

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

      std::cout << " text=" << text_path
                << " text_size=" << text.size()
                //<< " num_queries=" << queries.size()
                //<< " matches=" << matches
                //<< " errors=" << num_errors
                << "\n";
    }
  }

 private:
};

int main(int argc, char* argv[]) {
  index_benchmark benchmark;

  tlx::CmdlineParser cp;
  cp.set_description("Benchmark for text indices");
  cp.set_author("Alexander Herlez <alexander.herlez@tu-dortmund.de>\n");

  cp.add_param_string("text", benchmark.text_path, "Path to the input text");
  // cp.add_param_string("queries", benchmark.queries_path, "Path where queries");

  // cp.add_string('a', "algorithm", benchmark.algo, "Name of index");

  // cp.add_flag('c', "check", benchmark.check, "Check enumerated positions for pattern");

  cp.add_uint('r', "runs", benchmark.num_runs,
              "Number of runs that are used to "
              "report an average running time (default=5).");

  if (!cp.process(argc, argv)) {
    std::exit(EXIT_FAILURE);
  }
  std::cout << "r-index herlez------------------------------------------------------------------\n";
  benchmark.run<alx::r_index>("herlez");
  std::cout << "r-index prezza------------------------------------------------------------------\n";
  benchmark.run<ri::r_index>("prezza");
}
