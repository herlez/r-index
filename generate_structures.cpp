#include <libsais64.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <tdc/uint/uint40.hpp>

#include "util/io.hpp"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: gen_structs file_path.";
  }
  // load text
  std::filesystem::path text_path{argv[1]};
  std::string text = alx::io::load_text(text_path);

  // generate sa and write sa on disk
  {
    std::vector<int64_t> sa(text.size());
    libsais64(reinterpret_cast<const uint8_t *>(text.data()), sa.data(), text.size(), int64_t{0}, nullptr);
    std::filesystem::path sa_path = text_path;
    sa_path += ".sa";
    if (!std::filesystem::exists(sa_path)) {
      std::ofstream ostrm(sa_path, std::ios::binary);
      std::vector<tdc::uint40_t> sa_packed;
      for (size_t num : sa) {
        sa_packed.push_back(num);
      }
      ostrm.write(reinterpret_cast<const char *>(sa_packed.data()), sizeof(tdc::uint40_t) * sa_packed.size());
    }
  }

  // generate bwt and write bwt on disk
  {
    // last row of bwt
    std::vector<int64_t> sa(text.size());
    std::string bwt(text.size(), ' ');
    size_t primary_index = libsais64_bwt(reinterpret_cast<const uint8_t *>(text.data()), reinterpret_cast<uint8_t *>(bwt.data()), sa.data(), text.size(), int64_t{0}, nullptr);
    std::filesystem::path bwt_path = text_path;
    bwt_path += ".bwt";
    if (!std::filesystem::exists(bwt_path)) {
      std::ofstream ostrm(bwt_path, std::ios::binary);
      ostrm.write(bwt.data(), sizeof(char) * bwt.size());
    }

    // primary index of bwt
    std::filesystem::path primary_index_path = text_path;
    primary_index_path += ".prm";
    if (!std::filesystem::exists(primary_index_path)) {
      std::ofstream ostrm(primary_index_path, std::ios::binary);
      ostrm.write(reinterpret_cast<const char *>(&primary_index), sizeof(size_t) * 1);
    }
  }
  // generate .ri
  // generate .rix ?
}
