#pragma once

#include <fstream>
#include <string>
#include <vector>
#include "timer.hpp"
#include <iostream>
#include <filesystem>
#include "bwt.hpp"

namespace alx::io {

std::vector<std::string> read_strings_line_by_line(std::string const& path) {
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

std::string read_file(std::string const& path) {
  std::ifstream stream(path.c_str());

  return std::string((std::istreambuf_iterator<char>(stream)),
                     (std::istreambuf_iterator<char>()));
}

std::string get_text_name(std::string path) {
  auto last_slash = path.find_last_of('/');
  return path.substr(last_slash + 1);
}

std::string load_text(std::string path) {
  benchutil::timer timer;

  std::ifstream t(path);
  t.seekg(0, std::ios::end);
  size_t size = t.tellg();
  std::string buffer(size, ' ');
  t.seekg(0);
  t.read(&buffer[0], size);

  std::cout << "#READ file from=" << path << " size=" << buffer.size() << " time=" << timer.get_and_reset() << '\n';
  // std::cout << " (" << buffer.size() / 1'000'000 << " MB) in " << timer.get_and_reset() << "ms.\n";
  return buffer;
}

alx::bwt load_bwt(std::string path, std::string const& text) {
  benchutil::timer timer;
  alx::bwt bwt;
  if (std::filesystem::exists(path)) {
    bwt = bwt.load_from_file(path);
    std::cout << "#READ BWT from=" << path << " size=" << bwt.size() << " time=" << timer.get_and_reset() << '\n';
  } else {
    bwt = alx::bwt(text);
    std::cout << "#CALCULATE BWT size=" << bwt.size() << " time=" << timer.get_and_reset() << '\n';
    bwt.to_file(path);
    std::cout << "#STORE BWT at=" << path << " size=" << bwt.size() << " time=" << timer.get_and_reset() << '\n';
  }
  return bwt;
}

std::vector<std::string> load_queries(std::string path) {
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