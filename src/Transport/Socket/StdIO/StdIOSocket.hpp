#ifndef BABLE_LINUX_STDIOSOCKET_HPP
#define BABLE_LINUX_STDIOSOCKET_HPP

#define MAGIC_CODE 0xCAFE

#include <algorithm>
#include "../../AbstractSocket.hpp"
#include "../../../Log/Log.hpp"
#include "../../../utils/stream_formats.hpp"

class StdIOSocket : public AbstractSocket {

public:
  explicit StdIOSocket(std::shared_ptr<AbstractFormat> format)
      : AbstractSocket(std::move(format)) {
    m_header_length = 4;
    m_payload_length = 0;
    m_header.reserve(m_header_length);
  };

  bool send(const std::vector<uint8_t>& data) override {
    std::vector<uint8_t> result = generate_header(data);
    result.insert(result.end(), data.begin(), data.end());

    fwrite(result.data(), sizeof(uint8_t), result.size(), stdout);
    fflush(stdout);
    LOG.debug("Data sent.", "StdIOSocket");
    return true;
  };

  void poll(std::shared_ptr<uvw::Loop> loop, CallbackFunction on_received) override {
    auto poller = loop->resource<uvw::PipeHandle>(false);
    poller->open(STDIO_ID::in);
    poller->on<uvw::DataEvent>([this, on_received](const uvw::DataEvent& event, const uvw::PipeHandle& handle){
      LOG.debug("Readable data...", "StdIOSocket");
      if (!receive(reinterpret_cast<uint8_t*>(event.data.get()), event.length)) {
        return;
      }
      on_received(m_payload);
      clear();
    });

    poller->read();
  };

private:
  enum STDIO_ID {
    in = 0,
    out = 1
  };

  std::vector<uint8_t> generate_header(const std::vector<uint8_t>& data) {
    auto payload_length = static_cast<uint16_t>(data.size());

    std::vector<uint8_t> header;
    header.reserve(m_header_length);

    header.push_back(static_cast<uint8_t>(MAGIC_CODE >> 8));
    header.push_back(static_cast<uint8_t>(MAGIC_CODE & 0x00FF));

    // Use little endian
    header.push_back(static_cast<uint8_t>(payload_length & 0x00FF));
    header.push_back(static_cast<uint8_t>(payload_length >> 8));

    return header;
  };

  bool receive(const uint8_t* data, size_t length) {
    size_t consumed_data = 0;

    if (m_header.size() < m_header_length) {
      if (m_header.empty()) {
        consumed_data = std::min<size_t>(length, m_header_length);
        m_header.assign(data, data + consumed_data);
      } else {
        consumed_data = std::min<size_t>(length, m_header_length - m_header.size());
        m_header.insert(m_header.end(), data, data + consumed_data);
      }

      if (m_header.size() < m_header_length) {
        LOG.debug(std::to_string(m_payload.size()) + "/"  +  std::to_string(m_payload_length) + " bytes received (header)", "StdIOSocket");
        return false;
      }

      uint16_t magic_code = (m_header.at(0) << 8) | m_header.at(1);
      if (magic_code != MAGIC_CODE) {
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

    return true;
  };

  void clear() {
    m_header.clear();
    m_payload_length = 0;
    m_payload.clear();
    m_payload.shrink_to_fit();
  }

  std::vector<uint8_t> m_header;
  std::vector<uint8_t> m_payload;
  size_t m_header_length;
  size_t m_payload_length;

};

#endif //BABLE_LINUX_STDIOSOCKET_HPP