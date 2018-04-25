#ifndef BABLE_LINUX_STDIOSOCKET_HPP
#define BABLE_LINUX_STDIOSOCKET_HPP

#include "../AbstractSocket.hpp"
#include "../../Packet/constants.hpp"
#include "../../Packet/AbstractPacket.hpp"
#include "../../Serializer/Deserializer.hpp"

class StdIOSocket : public AbstractSocket {

public:
  explicit StdIOSocket(Packet::Type packet_type) {
    m_packet_type = packet_type;
  }

  const Packet::Type packet_type() const override {
    return m_packet_type;
  };

  uint8_t* format_packet(const std::unique_ptr<Packet::AbstractPacket>& packet) {
    switch(m_packet_type) {
      case Packet::ASCII:
        {
          std::string str = packet->to_ascii();
          return (uint8_t*)str.c_str();
        }

      default:
        throw std::invalid_argument("Can't format given packet to send it through standard input/output.");
    }
  }

  bool send(std::unique_ptr<Packet::AbstractPacket> packet) override {
    uint8_t* data = format_packet(packet);
    std::cout << data << std::endl;
    // TODO: check how to send data to stdout
//    fwrite(response.c_str(), 1, response.size(), stdout);
    fflush(stdout);
    return true;
  };

  Deserializer receive(const char* data, size_t length) override {
    Deserializer deser;
    deser.import((uint8_t*)(data), length);
    return deser;
  };

private:
  Packet::Type m_packet_type;

};

#endif //BABLE_LINUX_STDIOSOCKET_HPP
