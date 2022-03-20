#pragma once

#include <iostream>
#include <r_index.hpp>
#include <string>

#include "bwt.hpp"

namespace alx {
class r_index_prezza {
 public:
  r_index_prezza() {
    std::string empty = " ";
    m_index = ri::r_index<>(empty);
  }

  r_index_prezza(std::string& text) {
    if(text.size() == 0) {
      std::string empty = " ";
      m_index = ri::r_index<>(empty);
      return;
    }
    m_index = ri::r_index<>(text);
  }

  r_index_prezza(std::filesystem::path path) {
      std::ifstream input_stream(path);
      m_index.load(input_stream);
  }
  void index_to_file(std::filesystem::path path) {
      std::ofstream output_stream(path);
      m_index.serialize(output_stream);
  }

  size_t occ(std::string& pattern) {
    assert (pattern.size() != 0);
    //assert pattern is free of \0 and \1
    return m_index.occ(pattern);
  }

  std::vector<size_t> enumerate(std::string const& pattern) {
    std::vector<size_t> result;
    return result;
  }

 private:
 ri::r_index<> m_index;

};
}  // namespace alx
