#ifndef AMQP_RECEIVED_FRAME_H_
#define AMQP_RECEIVED_FRAME_H_

#include <cstdint>

namespace amqp {

class Buffer;
class ConnectionImpl;

class ReceivedFrame {
 public:
  ReceivedFrame(const Buffer& buffer, uint32_t max);
  virtual ~ReceivedFrame() {}

  bool Header() const;
  bool Complete() const;

  uint16_t channel() const { return channel_; }
  uint64_t total_size() const { return payload_size_ + 8; }
  uint32_t payload_size() const { return payload_size_; }

  uint8_t NextUInt8();
  int8_t NextInt8();
  uint16_t NextUInt16();
  int16_t NextInt16();
  uint32_t NextUInt32();
  int32_t NextInt32();
  uint64_t NextUInt64();
  int64_t NextInt64();
  float NextFloat();
  double NextDouble();
  const char* NextData(uint32_t size);
  
  bool Process(ConnectionImpl* connection);
  
 private:
  const Buffer& buffer_;
  uint32_t skip_;
  uint8_t type_;
  uint16_t channel_;
  uint32_t payload_size_;
  
  friend class FrameCheck;

  bool ProcessMethodFrame(ConnectionImpl* connection);
  bool ProcessConnectionFrame(ConnectionImpl* connection);
  bool ProcessChannelFrame(ConnectionImpl* connection);
  bool ProcessExchangeFrame(ConnectionImpl* connection);
  bool ProcessQueueFrame(ConnectionImpl* connection);
  bool ProcessBasicFrame(ConnectionImpl* connection);
  bool ProcessTransactionFrame(ConnectionImpl* connection);
  bool ProcessHeaderFrame(ConnectionImpl* connection);

};

} // namespace amqp
#endif // AMQP_RECEIVED_FRAME_H_
