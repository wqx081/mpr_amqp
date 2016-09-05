#ifndef AMQP_FIELD_PROXY_H_
#define AMQP_FIELD_PROXY_H_
#include "amqp/string_field.h"
#include "amqp/boolean_set.h"
#include "amqp/decimal_field.h"
#include "amqp/numeric_field.h"

#include <cstdint>
#include <string>

namespace amqp {

class Table;
class Array;
class Field;

template <
  typename T,
  typename I
>
class FieldProxy {
 public:
  FieldProxy(T* source, I index)
    : source_(source),
      index_(index) {}

  FieldProxy& operator=(bool value) {
    source_->Set(index_, BooleanSet(value));
    return *this;
  }

  FieldProxy& operator=(uint8_t value) {
    source_->Set(index_, UOctet(value));
    return *this;
  }

  FieldProxy& operator=(int8_t value) {
    source_->Set(index_, Octet(value));
    return *this;
  }

  FieldProxy& operator=(uint16_t value) {
    source_->Set(index_, UShort(value));
    return *this;
  }

  FieldProxy& operator=(int16_t value) {
    source_->Set(index_, Short(value));
    return *this;
  }

  FieldProxy& operator=(int32_t value) {
    source_->Set(index_, Long(value));
    return *this;
  }

  FieldProxy& operator=(uint32_t value) {
    source_->Set(index_, ULong(value));
    return *this;
  }

  FieldProxy& operator=(int64_t value) {
    source_->Set(index_, ULong(value));
    return *this;
  }

  FieldProxy& operator=(uint64_t value) {
    source_->Set(index_, ULong(value));
    return *this;
  }

  
 private:
  T* source_;
  I index_;
};

} // namespace amqp
#endif // AMQP_FIELD_PROXY_H_
