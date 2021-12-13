
#include <assert.h>

#include <bitset>
#include <cstdlib>
#include <vector>

#include "util/io.hpp"

std::string generate_text(size_t size, std::bitset<256> const& alphabet) {
  std::string text;

  std::vector<unsigned char> alphabet_vector;
  for (size_t c{0}; c < alphabet.size(); ++c) {
    if (alphabet[c]) {
      alphabet_vector.push_back(c);
    }
  }
  assert(alphabet.count() == alphabet_vector.size());

  assert(alphabet.count() >= size);
  for (auto c : alphabet_vector) {
    text.push_back(c);
  }

  std::srand(0);
  while (text.size() < size) {
    text.push_back(alphabet_vector[std::rand() % alphabet_vector.size()]);
  }

  return text;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  size_t text_size = size_t{1} << 26;

  {
    std::bitset<256> alphabet;
    alphabet[0] = true;
    alphabet[1] = true;
    std::string text = generate_text(text_size, alphabet);
    alx::io::save_text("sigma2_s", text);
  }
  {
    std::bitset<256> alphabet;
    alphabet[254] = true;
    alphabet[255] = true;
    std::string text = generate_text(text_size, alphabet);
    alx::io::save_text("sigma2_e", text);
  }
  {
    std::bitset<256> alphabet;
    for (size_t c{0}; c < 16; ++c) {
      alphabet[c] = true;
    }
    std::string text = generate_text(text_size, alphabet);
    alx::io::save_text("sigma16_s", text);
  }
  {
    std::bitset<256> alphabet;
    for (size_t c{64}; c < 80; ++c) {
      alphabet[c] = true;
    }
    std::string text = generate_text(text_size, alphabet);
    alx::io::save_text("sigma16_m", text);
  }
  {
    std::bitset<256> alphabet;
    for (size_t c{240}; c < 256; ++c) {
      alphabet[c] = true;
    }
    std::string text = generate_text(text_size, alphabet);
    alx::io::save_text("sigma16_e", text);
  }
  {
    std::bitset<256> alphabet;
    for (size_t c{0}; c < 128; ++c) {
      alphabet[c] = true;
    }
    std::string text = generate_text(text_size, alphabet);
    alx::io::save_text("sigma128_s", text);
  }
  {
    std::bitset<256> alphabet;
    for (size_t c{128}; c < 256; ++c) {
      alphabet[c] = true;
    }
    std::string text = generate_text(text_size, alphabet);
    alx::io::save_text("sigma128_e", text);
  }
  {
    std::bitset<256> alphabet;
    for (size_t c{0}; c < 256; ++c) {
      alphabet[c] = true;
    }
    std::string text = generate_text(text_size, alphabet);
    alx::io::save_text("sigma256", text);
  }
}
