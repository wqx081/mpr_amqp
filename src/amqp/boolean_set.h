#ifndef AMQP_BOOLEAN_SET_H_
#define AMQP_BOOLEAN_SET_H_

#include "amqp/field.h"
#include "amqp/out_buffer.h"
#include "amqp/received_frame.h"

#include <ostream>

namespace amqp {

class BooleanSet : public Field {
 public:
  BooleanSet(bool v0 = false,
             bool v1 = false,
             bool v2 = false,
             bool v3 = false,
             bool v4 = false,
             bool v5 = false,
             bool v6 = false,
             bool v7 = false) {
    byte_ = 0;
    Set(0, v0);
    Set(1, v1);
    Set(2, v2);
    Set(3, v3);
    Set(4, v4);
    Set(5, v5);
    Set(6, v6);
    Set(7, v7);
  }

  BooleanSet(ReceivedFrame& frame) {
    byte_ = frame.NextUInt8();
  }  

  BooleanSet(const BooleanSet& other) {
    byte_ = other.byte_;
  }

  virtual ~BooleanSet() {}

  virtual std::shared_ptr<Field> Clone() const override {
    return std::make_shared<BooleanSet>(*this);
  }

  virtual void Output(std::ostream& os) const override {
    os << "BooleanSet(";
    for (int i=0; i < 8; ++i) {
      os << (i == 0 ? "" : ",") << (Get(i) ? 1 : 0);
    }
    os << ")";
  }

  bool Get(uint32_t index) const {
    if (index > 7) return false;
    return 0 != ((1 << index) & byte_);
  }

  void Set(uint32_t index, bool v) {
    if (index > 7) return;

    if (v) {
      byte_ |= (1 << index);
    } else {
      byte_ &= ~(1 << index);
    }
  }

  virtual void Fill(OutBuffer& buffer) const override {
    buffer.Add(byte_);
  }

  uint8_t value() const {
    return byte_;
  }

  virtual char TypeId() const override {
    return 't';
  }

  bool IsBoolean() const override {
    return true;
  }

  virtual size_t size() const override {
    return 1;
  }

 private:
  uint8_t byte_;
};

} // namespace amqp

#endif // AMQP_BOOLEAN_SET_H_
