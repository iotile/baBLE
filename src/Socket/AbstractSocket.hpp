#ifndef BABLE_LINUX_ABSTRACTSOCKET_HPP
#define BABLE_LINUX_ABSTRACTSOCKET_HPP

#include "../Packet/AbstractPacket.hpp"
#include "../Serializer/Deserializer.hpp"
#include "../Packet/constants.hpp"

class AbstractSocket {

public:
  virtual const Packet::Type packet_type() const = 0;
  virtual bool send(std::unique_ptr<Packet::AbstractPacket> packet) {
    throw std::runtime_error("send(std::unique_ptr<Packet::AbstractPacket>) not defined.");
  };

  virtual Deserializer receive() {
    throw std::runtime_error("receive() not defined.");
  };

  virtual Deserializer receive(const char* data, size_t length) {
    throw std::runtime_error("receive(const char*, size_t) not defined.");
  };

  virtual ~AbstractSocket() = default;

};

#endif //BABLE_LINUX_ABSTRACTSOCKET_HPP
