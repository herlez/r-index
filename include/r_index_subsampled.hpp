#pragma once

#include <iostream>
#include <ri/r_index.h>
#include <string>

#include "bwt.hpp"

namespace alx {
class r_index_subsampled {
 public:
  r_index_subsampled() {
    m_index = ri::RIndex();
  }

  r_index_subsampled(std::string const& text) {
    if(text.size() == 0) {
      m_index = ri::r_index<>(" ");
      return;
    }
    m_index = ri::RIndex(text);
  }


  size_t occ(std::string const& pattern) {
    assert (pattern.size() != 0);
    return m_index.Occ(pattern);
  }

  std::vector<size_t> enumerate(std::string const& pattern) {
    std::vector<size_t> result;
    return result;
  }

 private:
 ri::RIndex m_index;

};
}  // namespace alx

