#ifndef AMQP_OUT_BUFFER_H_
#define AMQP_OUT_BUFFER_H_
#include <memory>
#include <cstring>

#include "base/byteorder.h"

namespace amqp {

class OutBuffer {
 public:

  OutBuffer(uint32_t capacity)
    : buffer_(new char[capacity]),
      current_(buffer_.get()),
      size_(0),
      capacity_(capacity) {}

  OutBuffer(OutBuffer&& other)
    : buffer_(std::move(other.buffer_)),
      current_(other.current_),
      size_(other.size_),
      capacity_(other.capacity_) {
    other.size_ = 0;
    other.capacity_ = 0;
    other.current_ = nullptr;
  }

  virtual ~OutBuffer() {}

  const char* data() const { return buffer_.get(); }  
  size_t size() const { return size_; }

  void Add(const char* str, uint32_t size) {
    // Not Check length???
    memcpy(current_, str, size);
    current_ += size;
    size_ += size;
  }

  void Add(const std::string& str) {
    Add(str.c_str(), str.size());
  }

  void Add(uint8_t value) {
    memcpy(current_, &value, sizeof(value));
    current_ += sizeof(value);
    size_ += sizeof(value);
  }

  void Add(uint16_t value) {
    uint16_t v = base::HostToNet16(value);
    memcpy(current_, &v, sizeof(v));
    current_ += sizeof(v);
    size_ += sizeof(v);
  }

  void Add(uint32_t value) {
    uint32_t v = base::HostToNet32(value);
    memcpy(current_, &v, sizeof(v));
    current_ += sizeof(v);
    size_ += sizeof(v);
  }

  void Add(uint64_t value) {
    uint64_t v = base::HostToNet64(value);
    memcpy(current_, &v, sizeof(v));
    current_ += sizeof(v);
    size_ += sizeof(v);
  }

  void Add(int8_t v) {
    memcpy(current_, &v, sizeof(v));
    current_ += sizeof(v);
    size_ += sizeof(v);
  }  

  void Add(int16_t value) {
    int16_t v = base::HostToNet16(value);
    memcpy(current_, &v, sizeof(v));
    current_ += sizeof(v);
    size_ += sizeof(v);
  }

  void Add(int32_t value) {
    int32_t v = base::HostToNet32(value);
    memcpy(current_, &v, sizeof(v));
    current_ += sizeof(v);
    size_ += sizeof(v);
  }

  void Add(int64_t value) {
    int64_t v = base::HostToNet64(value);
    memcpy(current_, &v, sizeof(v));
    current_ += sizeof(v);
    size_ += sizeof(v);
  }

  void Add(float v) {
    memcpy(current_, &v, sizeof(v));
    current_ += sizeof(v);
    size_ += sizeof(v);
  }

  void Add(double v) {
    memcpy(current_, &v, sizeof(v));
    current_ += sizeof(v);
    size_ += sizeof(v);
  }

 private:
  std::unique_ptr<char[]> buffer_;
  char* current_;
  size_t size_;
  size_t capacity_;
};

} // namespace amqp
#endif // AMQP_OUT_BUFFER_H_
