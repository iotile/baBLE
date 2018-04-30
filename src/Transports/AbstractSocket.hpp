#ifndef BABLE_LINUX_ABSTRACTSOCKET_HPP
#define BABLE_LINUX_ABSTRACTSOCKET_HPP

#include <uvw.hpp>
#include "../Packet/AbstractPacket.hpp"
#include "../Serializer/Deserializer.hpp"
#include "../Packet/constants.hpp"
#include "../Log/Log.hpp"

class AbstractSocket {

public:
  using CallbackFunction = std::function<void(const std::vector<uint8_t>&)>;

  virtual bool send(const std::vector<uint8_t>& data) = 0;
  virtual void poll(std::shared_ptr<uvw::Loop> loop, CallbackFunction on_received) = 0;

  virtual ~AbstractSocket() = default;

protected:
  explicit AbstractSocket(size_t header_size) {
    m_received_payload_length = 0;
    m_received_header.reserve(header_size);
  };

  inline const uint16_t get_magic_code() const {
    return 0xCAFE;
  }

  inline virtual const size_t get_header_size() const {
    throw std::runtime_error("get_header_size() not defined.");
  };

  virtual bool receive_with_header(const uint8_t* data, size_t length) {
    size_t consumed_data = 0;

    if (m_received_header.size() < get_header_size()) {
      if (m_received_header.empty()) {
        consumed_data = std::min<size_t>(length, get_header_size());
        m_received_header.assign(data, data + consumed_data);
      } else {
        consumed_data = std::min<size_t>(length, get_header_size() - m_received_header.size());
        m_received_header.insert(m_received_header.end(), data, data + consumed_data);
      }

      if (m_received_header.size() < get_header_size()) {
        LOG.debug(std::to_string(m_received_payload.size()) + "/"  +  std::to_string(m_received_payload_length) + " bytes received (header)", "StdIOSocket");
        return false;
      }

      uint16_t magic_code = (m_received_header.at(0) << 8) | m_received_header.at(1);
      if (magic_code != get_magic_code()) {
        LOG.error("Packet with wrong magic code received: " + std::to_string(magic_code));
        m_received_header.clear();
        return false;
      }

      if (__BYTE_ORDER == __LITTLE_ENDIAN) {
        m_received_payload_length = (m_received_header.at(3) << 8) | m_received_header.at(2);
      } else {
        m_received_payload_length = (m_received_header.at(2) << 8) | m_received_header.at(3);
      }
    }

    m_received_payload.insert(m_received_payload.end(), data + consumed_data, data + length);

    if (m_received_payload.size() < m_received_payload_length) {
      LOG.debug(std::to_string(m_received_payload.size()) + "/"  +  std::to_string(m_received_payload_length) + " bytes received", "StdIOSocket");
      return false;
    }

    return true;
  }

  void clear() {
    m_received_header.clear();
    m_received_payload_length = 0;
    m_received_payload.clear();
    m_received_payload.shrink_to_fit();
  }

  std::vector<uint8_t> m_received_header;
  std::vector<uint8_t> m_received_payload;
  size_t m_received_payload_length;

};

#endif //BABLE_LINUX_ABSTRACTSOCKET_HPP
