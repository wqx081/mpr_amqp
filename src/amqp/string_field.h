#ifndef AMQP_STRING_FIELD_H_
#define AMQP_STRING_FIELD_H_

#include "amqp/field.h"
#include "amqp/numeric_field.h"
#include "amqp/received_frame.h"
#include "amqp/out_buffer.h"

namespace amqp {

template <
  typename T,
  char F
>
class StringField : public Field {
 public:

  StringField() {}
  StringField(const std::string& value) : data_(value) {}
  StringField(std::string&& value) : data_(std::move(value)) {}  

  StringField(ReceivedFrame& frame) {
    T size(frame);
    data_ = std::string(frame.NextData(size.value()), (size_t)size.value());
  }

  virtual ~StringField() {}

  virtual std::shared_ptr<Field> Clone() const override {
    return std::make_shared<StringField>(data_);
  }

  StringField& operator=(const std::string& value) {
    data_ = value;
    return *this;
  }

  StringField& operator=(std::string&& value) {
    data_ = std::move(value);
    return *this;
  }

  virtual size_t size() const override {
    T size(data_.size());
    return size.size() + data_.size();
  }

  virtual operator const std::string& () const override {
    return data_;
  }

  const std::string& value() const { return data_; }

  constexpr static size_t MaxLength() { return T::max(); }

  virtual void Fill(OutBuffer& buffer) const override {
    T size(data_.size());
    size.Fill(buffer);
    buffer.Add(data_);
  }

  virtual char TypeId() const override {
    return F;
  }

  bool IsString() const override {
    return true;
  }

  virtual void Output(std::ostream& os) const override {
    os << "string(" << value() << ")";
  }

 private:
  std::string data_;
};

typedef StringField<UOctet, 's'>    ShortString;
typedef StringField<ULong, 'S'>     LongString;

} // namespace amqp
#endif // AMQP_STRING_FIELD_H_
