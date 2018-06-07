#ifndef BABLE_LINUX_STDIOSOCKET_HPP
#define BABLE_LINUX_STDIOSOCKET_HPP

#define MAGIC_CODE 0xCAFE

#include "../../AbstractSocket.hpp"

enum STDIO_ID {
  in = 0,
  out = 1
};

class StdIOSocket : public AbstractSocket {

public:
  using OnCloseCallback = std::function<void()>;

  explicit StdIOSocket(std::shared_ptr<uvw::Loop>& loop, std::shared_ptr<AbstractFormat> format);

  bool send(const std::vector<uint8_t>& data) override;
  void poll(OnReceivedCallback on_received, OnErrorCallback on_error) override;
  void on_close(OnCloseCallback on_close);

private:
  std::vector<uint8_t> generate_header(const std::vector<uint8_t>& data);
  bool receive(const uint8_t* data, size_t length);
  void clear();

  std::vector<uint8_t> m_header;
  std::vector<uint8_t> m_payload;
  size_t m_header_length;
  size_t m_payload_length;

  std::shared_ptr<uvw::PipeHandle> m_poller;

};

#endif //BABLE_LINUX_STDIOSOCKET_HPP
