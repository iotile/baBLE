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

  std::vector<uint8_t> format_packet(const std::unique_ptr<Packet::AbstractPacket>& packet) {
    switch(m_packet_type) {
      case Packet::ASCII:
        {
          std::string str = packet->to_ascii() + "\n";
          std::vector<uint8_t> result = std::vector<uint8_t>(str.begin(), str.end());
          return result;
        }

      default:
        throw std::invalid_argument("Can't format given packet to send it through standard input/output.");
    }
  }

  bool send(std::unique_ptr<Packet::AbstractPacket> packet) override {
    std::vector<uint8_t> formatted_packet = format_packet(packet);
    fwrite(formatted_packet.data(), sizeof(uint8_t), formatted_packet.size(), stdout);
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
