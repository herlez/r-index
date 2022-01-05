#pragma once

#include <array>
#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "timer.hpp"

namespace alx::io {

std::vector<std::string> read_strings_line_by_line(std::filesystem::path const& path) {
  std::fstream stream(path.c_str(), std::ios::in);
  std::string cur_line;

  std::vector<std::string> result;

  if (stream) {
    while (std::getline(stream, cur_line)) {
      result.push_back(cur_line);
    }
  }
  stream.close();

  return result;
}

std::string load_text(std::filesystem::path const& path, bool timer_output = false) {
  if (!std::filesystem::exists(path)) {
    std::cout << "#FILE " << path << " not found.\n";
    return std::string{};
  }

  benchutil::timer timer;

  std::ifstream t(path);
  t.seekg(0, std::ios::end);
  size_t size = t.tellg();
  std::string buffer(size, ' ');
  t.seekg(0);
  t.read(&buffer[0], size);

  if (timer_output) {
    std::cout << "#READ file from=" << path << " size=" << buffer.size() << " time=" << timer.get_and_reset() << '\n';
  }
  // std::cout << " (" << buffer.size() / 1'000'000 << " MB) in " << timer.get_and_reset() << "ms.\n";
  return buffer;
}

void save_text(std::filesystem::path const& path, std::string const& text, bool timer_output = false) {
  benchutil::timer timer;

  std::ofstream of(path);
  of << text;

  if (timer_output) {
    std::cout << "#WRITE file to=" << path << " size=" << text.size() << " time=" << timer.get_and_reset() << '\n';
  }
}

std::array<size_t, 256> histogram(std::string const& text) {
  std::array<size_t, 256> hist;
  hist.fill(0);
  for (unsigned char c : text) {
    ++hist[c];
  }
  return hist;
}

size_t alphabet_size(std::string const& text) {
  std::array<size_t, 256> hist = histogram(text);
  size_t alphabet_size = 0;
  for (size_t a : hist) {
    alphabet_size += (a > 0);
  }
  return alphabet_size;
}

std::vector<std::string> load_queries(std::filesystem::path const& path) {
  benchutil::timer timer;

  std::vector<std::string> queries;
  std::ifstream ifstr(path);
  std::string line;
  while (std::getline(ifstr, line)) {
    queries.push_back(line);
  }

  std::cout << "#READ queries from=" << path << " size=" << queries.size() << " in=" << timer.get_and_reset() << '\n';
  return queries;
}

std::vector<std::string> generate_queries(std::string const& text, size_t num, size_t length) {
  benchutil::timer timer;

  std::vector<std::string> queries;
  for (size_t i = 0; i < num; ++i) {
    size_t q_pos = i * 100;
    if (q_pos + length >= text.size()) {
      break;
    }
    queries.push_back(text.substr(q_pos, length));
  }
  std::cout << "#GENERATE queries size=" << queries.size() << " time=" << timer.get_and_reset() << '\n';
  return queries;
}

}  // namespace alx::io