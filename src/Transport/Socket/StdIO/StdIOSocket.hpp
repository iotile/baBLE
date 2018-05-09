#ifndef BABLE_LINUX_STDIOSOCKET_HPP
#define BABLE_LINUX_STDIOSOCKET_HPP

#define MAGIC_CODE 0xCAFE

#include <algorithm>
#include "../../AbstractSocket.hpp"
#include "../../../Log/Log.hpp"

enum STDIO_ID {
  in = 0,
  out = 1
};

class StdIOSocket : public AbstractSocket {

public:
  explicit StdIOSocket(std::shared_ptr<AbstractFormat> format);

  bool send(const std::vector<uint8_t>& data) override;

  void poll(std::shared_ptr<uvw::Loop> loop, CallbackFunction on_received) override;

private:
  std::vector<uint8_t> generate_header(const std::vector<uint8_t>& data);

  bool receive(const uint8_t* data, size_t length);

  void clear();

  std::vector<uint8_t> m_header;
  std::vector<uint8_t> m_payload;
  size_t m_header_length;
  size_t m_payload_length;

};

#endif //BABLE_LINUX_STDIOSOCKET_HPP
