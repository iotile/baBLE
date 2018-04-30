#ifndef BABLE_LINUX_STDIOSOCKET_HPP
#define BABLE_LINUX_STDIOSOCKET_HPP

#include <algorithm>
#include "../AbstractSocket.hpp"
#include "../../Log/Log.hpp"

class StdIOSocket : public AbstractSocket {

public:
  explicit StdIOSocket(): AbstractSocket(get_header_size()) {}

  bool send(const std::vector<uint8_t>& data) override {
    std::vector<uint8_t> result = generate_header(data);
    result.insert(result.end(), data.begin(), data.end());

    fwrite(result.data(), sizeof(uint8_t), result.size(), stdout);
    fflush(stdout);
    LOG.debug("Data sent.", "StdIOSocket");
    return true;
  };

  std::vector<uint8_t> generate_header(const std::vector<uint8_t>& data) {
    auto payload_length = static_cast<uint16_t>(data.size());

    std::vector<uint8_t> header;
    header.reserve(get_header_size());

    // Use little endian
    header.push_back(static_cast<uint8_t>(get_magic_code() & 0x00FF));
    header.push_back(static_cast<uint8_t>(get_magic_code() >> 8));

    header.push_back(static_cast<uint8_t>(payload_length & 0x00FF));
    header.push_back(static_cast<uint8_t>(payload_length >> 8));

    return header;
  };

  void poll(std::shared_ptr<uvw::Loop> loop, CallbackFunction on_received) override {
    auto poller = loop->resource<uvw::PipeHandle>(false);
    poller->open(STDIO_ID::in);
    poller->on<uvw::DataEvent>([this, &on_received](const uvw::DataEvent& event, const uvw::PipeHandle& handle){
      LOG.debug("Readable data...", "StdIOSocket");
      if (!receive_with_header(reinterpret_cast<uint8_t*>(event.data.get()), event.length)) {
        return;
      }
      on_received(m_received_payload);
      clear();
    });

    poller->read();
  }

private:
  inline const size_t get_header_size() const override {
    return 4;
  }

  enum STDIO_ID {
    in = 0,
    out = 1
  };

  std::vector<uint8_t> m_received_header;
  std::vector<uint8_t> m_received_payload;
  size_t m_received_payload_length;
};

#endif //BABLE_LINUX_STDIOSOCKET_HPP
