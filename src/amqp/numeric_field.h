#ifndef AMQP_NUMERIC_FIELD_H_
#define AMQP_NUMERIC_FIELD_H_
#include "amqp/received_frame.h"
#include "amqp/out_buffer.h"
#include "amqp/field.h"

#include <memory>
#include <ostream>

namespace amqp {

template <
  typename T,
  char F,
  typename = typename std::enable_if<std::is_arithmetic<T>::value, T>,
  typename = typename std::enable_if<std::is_integral<T>::value, T>
>
class NumericField : public Field {
 public:
  NumericField() : value_(0) {}
  NumericField(T value) : value_(value) {}

  NumericField(ReceivedFrame& frame) {
    if (std::is_same<int8_t, typename std::remove_cv<T>::type>::value) {
      value_ = frame.NextInt8();
    } else if (std::is_same<int16_t, typename std::remove_cv<T>::type>::value) {
      value_ = frame.NextInt16();
    } else if (std::is_same<int32_t, typename std::remove_cv<T>::type>::value) {
      value_ = frame.NextInt32();
    } else if (std::is_same<int64_t, typename std::remove_cv<T>::type>::value) {
      value_ = frame.NextInt64();
    } else if (std::is_same<uint8_t, typename std::remove_cv<T>::type>::value) {
      value_ = frame.NextUInt8();
    } else if (std::is_same<uint16_t, typename std::remove_cv<T>::type>::value) {
      value_ = frame.NextUInt16();
    } else if (std::is_same<uint32_t, typename std::remove_cv<T>::type>::value) {
      value_ = frame.NextUInt32();
    } else if (std::is_same<uint64_t, typename std::remove_cv<T>::type>::value) {
      value_ = frame.NextUInt64();
    } else if (std::is_same<float, typename std::remove_cv<T>::type>::value) {
      value_ = frame.NextFloat();
    } else if (std::is_same<double, typename std::remove_cv<T>::type>::value) {
      value_ = frame.NextDouble();
    }

  }

  virtual ~NumericField() {}

  virtual std::shared_ptr<Field> Clone() const override {
    return std::make_shared<NumericField>(value_);
  } 

  NumericField& operator=(T value) {
    value_ = value;
    return *this;
  }

  virtual operator int8_t  () const override { return value_; }
  virtual operator int16_t () const override { return value_; }
  virtual operator int32_t () const override { return value_; }
  virtual operator int64_t () const override { return value_; }
  virtual operator uint8_t  () const override { return value_; }
  virtual operator uint16_t () const override { return value_; }
  virtual operator uint32_t () const override { return value_; }
  virtual operator uint64_t () const override { return value_; }

  T value() const { return value_; }

  virtual bool IsInteger() const override { return true; }
  virtual size_t size() const override { return sizeof(value_); }  

  virtual void Fill(OutBuffer& buffer) const override {
    T value = value_;
    buffer.Add(value);
  }

  virtual char TypeId() const override {
    return F;
  }

  virtual void Output(std::ostream& os) const override {
    os << "numeric(" << value() << ")";
  }

 private:
  T value_;
};

typedef NumericField<int8_t, 'b'>   Octet;
typedef NumericField<uint8_t, 'B'>  UOctet;
typedef NumericField<int16_t, 'U'>  Short;
typedef NumericField<uint16_t, 'u'> UShort;
typedef NumericField<int32_t, 'I'>  Long;
typedef NumericField<uint32_t, 'i'> ULong;

typedef NumericField<int64_t, 'L'>  LongLong;
typedef NumericField<uint64_t, 'l'> ULongLong;
typedef NumericField<uint64_t, 'T'> Timestamp;

typedef NumericField<float, 'f'>    Float;
typedef NumericField<double, 'd'>   Double;

} // namespace amqp
#endif // AMQP_NUMERIC_FIELD_H_
