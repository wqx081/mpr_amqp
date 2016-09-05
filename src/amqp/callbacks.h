#ifndef AMQP_AMQP_CALLBACKS_H_
#define AMQP_AMQP_CALLBACKS_H_
#include <string>
#include <functional>

namespace amqp {

class Message;
class MetaData;

using SuccessCallback   = std::function<void()>;
using ErrorCallback     = std::function<void(const char* message)>;
using FinalizeCallback  = std::function<void()>;
using EmptyCallback     = std::function<void()>;
using BeginCallback     = std::function<void()>;
using HeaderCallback    = std::function<void(const MetaData& meta_data)>;
using DataCallback      = std::function<void(const char* data, size_t size)>;
using MessageCallback   = std::function<void(Message&& message, 
                                             uint64_t delivery_tag,
                                             bool redelivered)>;

using CompleteCallback  = std::function<void(uint64_t delivery_tag,
                                             bool redelivered)>;
using QueueCallback     = std::function<void(const std::string& name,
                                             uint32_t message_count,
                                             uint32_t consumer_count)>;

using DeleteCallback    = std::function<void(uint32_t deleted_messages)>;
using SizeCallback      = std::function<void(uint32_t message_count)>;
using ConsumeCallback   = std::function<void(const std::string& consumer)>;
using CancelCallback    = std::function<void(const std::string& consumer)>;

} // namespace amqp
#endif // AMQP_AMQP_CALLBACKS_H_
