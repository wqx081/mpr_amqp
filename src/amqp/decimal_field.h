#ifndef AMQP_DECIMAL_FIELD_H_
#define AMQP_DECIMAL_FIELD_H_
#include "amqp/field.h"
#include "amqp/out_buffer.h"
#include "amqp/received_frame.h"

#include <cmath>
#include <ostream>

namespace amqp {

class DecimalField : public Field {
 public:
  DecimalField(uint8_t places = 0, uint32_t number = 0)
    : places_(places),
      number_(number) {}

  DecimalField(ReceivedFrame& frame) {
    places_ = frame.NextUInt8();
    number_ = frame.NextUInt32();
  }

  virtual ~DecimalField() {}

  virtual std::shared_ptr<Field> Clone() const override {
    return std::make_shared<DecimalField>(places_, number_);
  }

  virtual void Output(std::ostream& os) const override {
    os << "decimal(" << number_ / pow(10.0f, places_) << ")";
  }

  DecimalField& operator=(const DecimalField& v) {
    if (this == &v) return *this;

    places_ = v.places_;
    number_ = v.number_;

    return *this;
  }

  virtual operator double() const override { return number_ / pow(10.0f, places_); }
  virtual operator float() const override  { return number_ / pow(10.0f, places_); }

  bool operator==(const DecimalField& value) const {
    return number_ == value.number() && places_ == value.places();
  }

  bool operator!=(const DecimalField& value) const {
    return !(*this == value);
  }

  virtual size_t size() const override {
    return 5;
  }

  virtual bool IsDecimal() const override {
    return true;
  }

  virtual char TypeId() const override {
    return 'D';
  }

  DecimalField value() const {
    return *this;
  }

  uint8_t places() const { 
    return places_;                          
  }

  uint32_t number() const {
    return number_;
  }

 protected:

  virtual void Fill(OutBuffer& buffer) const override {
    buffer.Add(places_);
    buffer.Add(number_);
  }

 private:
  uint8_t places_;
  uint32_t number_;
};

} // namespace amqp
#endif // AMQP_DECIMAL_FIELD_H_
