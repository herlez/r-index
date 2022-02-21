#pragma once

#include <libsais64.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

namespace alx {

struct bwt {
  size_t primary_index;
  std::string last_row;

  bwt() = default;

  bwt(std::string const& text) {
    last_row.resize(text.size());
    std::vector<int64_t> sa(text.size() + 100);
    primary_index = libsais64_bwt(reinterpret_cast<const uint8_t*>(text.data()), reinterpret_cast<uint8_t*>(last_row.data()), sa.data(), text.size(), int64_t{100}, nullptr);
  }

  bwt(std::filesystem::path const& last_row_path, std::filesystem::path const& primary_index_path) {
    // If file does not exist, return empty string.
    if (!std::filesystem::exists(last_row_path)) {
      std::cout << last_row_path << " does not exist.";
      return;
    }
    if (!std::filesystem::exists(primary_index_path)) {
      std::cout << primary_index_path << " does not exist.";
      return;
    }

    
    // Read primary index.
    {
      std::ifstream in(primary_index_path, std::ios::binary);
      in.read(reinterpret_cast<char*>(&primary_index), sizeof(primary_index));
    }
    // Read last row.
    {
      size_t size = std::filesystem::file_size(last_row_path);
      last_row.resize(size);
      std::ifstream in(last_row_path, std::ios::binary);
      in.read(last_row.data(), size);
    }
  }

  std::string to_text() const {
    std::string text(last_row.size(), ' ');
    std::vector<int64_t> sa(text.size() + 1);
    libsais64_unbwt(reinterpret_cast<const uint8_t*>(last_row.data()), reinterpret_cast<uint8_t*>(text.data()), sa.data(), text.size(), nullptr, primary_index);
    return text;
  }

  int to_file(std::filesystem::path path) const {
    // Check whether bwt is already written
    if (std::filesystem::exists(path)) {
      return -1;
    }
    std::cout << "Write to " << path;
    // Write bwt
    std::ofstream out(path, std::ios::binary);
    //out << primary_index;
    out.write(reinterpret_cast<const char *>(&primary_index), sizeof(primary_index));
    out << last_row;
    return 0;
  }

  static bwt load_from_file(std::filesystem::path const& last_row_path, std::filesystem::path const& primary_index_path) {
    alx::bwt bwt;
    // If file does not exist, return empty string.
    if (!std::filesystem::exists(last_row_path)) {
      std::cout << last_row_path << " does not exist.";
      return bwt;
    }
    if (!std::filesystem::exists(primary_index_path)) {
      std::cout << primary_index_path << " does not exist.";
      return bwt;
    }

    
    // Read primary index.
    {
      std::ifstream in(primary_index_path, std::ios::binary);
      in.read(reinterpret_cast<char*>(&bwt.primary_index), sizeof(bwt.primary_index));
    }
    // Read last row.
    {
      size_t size = std::filesystem::file_size(last_row_path);
      bwt.last_row.resize(size);
      std::ifstream in(last_row_path, std::ios::binary);
      in.read(bwt.last_row.data(), size);
    }
    return bwt;
  }

  template <typename SA_Container>
  static bwt bwt_from_sa(SA_Container const& sa, std::string const& text) {
    assert(sa.size() == text.size());
    alx::bwt bwt;
    bwt.last_row.reserve(text.size());
    bwt.last_row.push_back(text.back()); // In first_row $ is first.
  
    for(size_t i{0}; i < sa.size(); ++i) {
      if(sa[i] == 0) [[unlikely]]{
        bwt.primary_index = i;
      } else {
        bwt.last_row.push_back(text[sa[i]-1]);
      }
    }
    assert(bwt.size() == text.size());
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
