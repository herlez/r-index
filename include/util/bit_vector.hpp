#pragma once

class BitAccess {

  uint64_t * const data_;
  size_t const pos_;
  
public:
  BitAccess(uint64_t * const data, size_t const pos) : data_(data),
						       pos_(pos) { }
  operator bool() const {
    uint8_t const offset = uint64_t(pos_) & uint64_t(0b111111);
    return (data_[pos_ >> 6] >> offset) & 1ULL;
  }

  BitAccess& operator=(bool const value) {
    uint64_t const mask = 1ULL << (uint64_t(pos_) & uint64_t(0b111111));
    data_[pos_ >> 6] = (data_[pos_ >> 6] & ~mask) | (-value & mask);
    return *this;
  }
}; // class BitAccess

class BitVector {

  friend class Rank;

  size_t const bit_size_;
  size_t const size_;
  //uint64_t * const data_;
  std::vector<uint64_t> data_;
  
public:

  BitVector(size_t const size) : bit_size_(size),
				 size_((bit_size_ >> 6) +
				       ((bit_size_ % 512 > 0) ? 8 : 0)),
				 data_(std::vector<uint64_t>(size_)) {
    //std::fill(data_.begin(), data_.end() + size_, 0ULL);
  }

  //~BitVector() {
  //  delete[] data_;
  //}

  BitAccess operator[](size_t const idx) {
    return BitAccess(data_.data(), idx);
  }

  
  void set_bit(size_t const position, bool const value) {
    // https://graphics.stanford.edu/~seander/bithacks.html
    // (ConditionalSetOrClearBitsWithoutBranching)
    uint64_t const mask = 1ULL << (uint64_t(position) & uint64_t(0b111111));
    data_[position >> 6] = (data_[position >> 6] & ~mask) | (-value & mask);
  }

  bool get_bit(size_t const position) const {
    uint8_t const offset = uint64_t(position) & uint64_t(0b111111);
    return (data_[position >> 6] >> offset) & 1ULL;
  }

  // returns the number of bits stored in the bit vector
  size_t size() const {
    return bit_size_;
  }
}; // class BitVector
