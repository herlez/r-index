#pragma once

#include <libsais64.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace alx {

struct bwt {
  size_t primary_index;
  std::string last_row;

  bwt() = default;

  bwt(std::string const& text) {
    last_row.resize(text.size());
    std::vector<int64_t> sa(text.size() + 100);
    primary_index = libsais64_bwt(reinterpret_cast<const uint8_t*>(text.data()), reinterpret_cast<uint8_t*>(last_row.data()), sa.data(), text.size(), int64_t{101}, nullptr);
  }

  std::string to_text() const {
    std::string text(last_row.size(), ' ');
    std::vector<int64_t> sa(text.size() + 1);
    libsais64_unbwt(reinterpret_cast<const uint8_t*>(last_row.data()), reinterpret_cast<uint8_t*>(text.data()), sa.data(), text.size(), nullptr, primary_index);
    return text;
  }

  int to_file(std::string const& path) const {
    // Check whether bwt is already written
    if (std::filesystem::exists(path)) {
      return -1;
    }

    // Write bwt
    std::ofstream out(path, std::ios::binary);
    //out << primary_index;
    out.write(reinterpret_cast<const char *>(&primary_index), sizeof(primary_index));
    out << last_row;
    return 0;
  }

  static bwt load_from_file(std::string const& path) {
    // If file does not exist, return empty string
    if (!std::filesystem::exists(path)) {
      return std::string{};
    }
    // Read bwt
    alx::bwt bwt;
    std::ifstream in(path, std::ios::binary);
    in.read(reinterpret_cast<char *>(&bwt.primary_index), sizeof(bwt.primary_index));
    //in >> bwt.primary_index;

    in.seekg(8, std::ios::beg);
    std::streampos begin = in.tellg();
    in.seekg(0, std::ios::end);
    size_t size = in.tellg() - begin;
    bwt.last_row.resize(size);
    in.seekg(8, std::ios::beg);
    in.read(bwt.last_row.data(), size);
    return bwt;
  }

  size_t size() const {
    return last_row.size();
  }

  /*char& operator[](size_t i) {
    return last_row[i];
  }*/
};
}  // namespace alx
