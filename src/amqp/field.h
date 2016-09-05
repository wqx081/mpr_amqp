//@Author: WangQixiang
//@Date:
//
// Design pattern: ProtoType
#ifndef AMQP_AMQP_FILED_H_
#define AMQP_AMQP_FILED_H_
#include <memory>

namespace amqp {

class ReceivedFrame;
class OutBuffer;
class Array;
class Table;

class Field {
 public:
  virtual ~Field() {}
  virtual std::shared_ptr<Field> Clone() const = 0;
  virtual size_t size() const = 0;
  virtual void Fill(OutBuffer& buffer) const = 0;
  virtual char TypeId() const = 0;
  virtual void Output(std::ostream& stream) const = 0;

  // For Casting
  virtual operator const std::string& () const;
  virtual operator const char* () const { return nullptr; }
  virtual operator int8_t  () const { return 0; } 
  virtual operator int16_t () const { return 0; }
  virtual operator int32_t () const { return 0; }
  virtual operator int64_t () const { return 0; }
  virtual operator uint8_t  () const { return 0; } 
  virtual operator uint16_t () const { return 0; }
  virtual operator uint32_t () const { return 0; }
  virtual operator uint64_t () const { return 0; }
  virtual operator float() const { return 0.0; }
  virtual operator double() const { return 0.0; }
  virtual operator const Array& () const;
  virtual operator const Table& () const;

  virtual bool IsInteger() const { return false; }
  virtual bool IsDecimal() const { return false; }
  virtual bool IsArray()   const { return false; }
  virtual bool IsTable()   const { return false; }
  virtual bool IsBoolean() const { return false; }
  virtual bool IsString()  const { return false; }

 protected:
  static Field* Decode(ReceivedFrame& frame);
};

inline std::ostream& operator<<(std::ostream& os, const Field& field) {
  field.Output(os);
  return os;
}

} // namespace amqp
#endif // AMQP_AMQP_FILED_H_
