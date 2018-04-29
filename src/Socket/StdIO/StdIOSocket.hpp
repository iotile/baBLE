#ifndef BABLE_LINUX_STDIOSOCKET_HPP
#define BABLE_LINUX_STDIOSOCKET_HPP

#define HEADER_LENGTH 4

#include <algorithm>
#include "../AbstractSocket.hpp"
#include "../../Packet/constants.hpp"
#include "../../Packet/AbstractPacket.hpp"
#include "../../Serializer/Deserializer.hpp"
#include "../../Log/Log.hpp"

class StdIOSocket : public AbstractSocket {

public:
  explicit StdIOSocket(Packet::Type packet_type) {
    m_packet_type = packet_type;
    m_payload_length = 0;
    m_header.reserve(4);
  }

  const Packet::Type packet_type() const override {
    return m_packet_type;
  };

  std::vector<uint8_t> format_packet(const std::unique_ptr<Packet::AbstractPacket>& packet) {
    switch(m_packet_type) {
      case Packet::ASCII:
        {
          std::string str = packet->to_ascii() + "\n";
          LOG.debug(str, "StdIOSocket");
          std::vector<uint8_t> result = std::vector<uint8_t>(str.begin(), str.end());
          return result;
        }

      case Packet::FLATBUFFERS:
        {
          Serializer ser = packet->to_flatbuffers();
          LOG.debug(ser, "StdIOSocket");
          return ser.raw_buffer();
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

  bool send(const std::string& str) {
    std::string data_str = str + "\n";
    fwrite(data_str.c_str(), sizeof(char), data_str.size(), stdout);
    fflush(stdout);
    return true;
  };

  bool receive(Deserializer& deser, const char* data, size_t length) {
    size_t consumed_data = 0;

    if (m_header.size() < HEADER_LENGTH) {
      if (m_header.empty()) {
        consumed_data = std::min<size_t>(length, HEADER_LENGTH);
        m_header.assign(data, data + consumed_data);
      } else {
        consumed_data = std::min<size_t>(length, HEADER_LENGTH - m_header.size());
        m_header.insert(m_header.end(), data, data + consumed_data);
      }

      if (m_header.size() < HEADER_LENGTH) {
        LOG.debug(std::to_string(m_payload.size()) + "/"  +  std::to_string(m_payload_length) + " bytes received (header)", "StdIOSocket");
        return false;
      }

      uint16_t magic_code = (m_header.at(0) << 8) | m_header.at(1);
      if (magic_code != 0xCAFE) {
        LOG.error("Packet with wrong magic code received: " + std::to_string(magic_code));
        m_header.clear();
        return false;
      }

      if (__BYTE_ORDER == __LITTLE_ENDIAN) {
        m_payload_length = (m_header.at(3) << 8) | m_header.at(2);
      } else {
        m_payload_length = (m_header.at(2) << 8) | m_header.at(3);
      }
    }

    m_payload.insert(m_payload.end(), data + consumed_data, data + length);

    if (m_payload.size() < m_payload_length) {
      LOG.debug(std::to_string(m_payload.size()) + "/"  +  std::to_string(m_payload_length) + " bytes received", "StdIOSocket");
      return false;
    }

    deser.import(m_payload);
    m_header.clear();
    m_payload.clear();
    m_payload.shrink_to_fit();
    m_payload_length = 0;
    return true;
  };

private:
  Packet::Type m_packet_type;
  std::vector<uint8_t> m_header;
  std::vector<uint8_t> m_payload;
  size_t m_payload_length;
};

#endif //BABLE_LINUX_STDIOSOCKET_HPP
