#ifndef BABLE_STDIOSOCKET_HPP
#define BABLE_STDIOSOCKET_HPP

#define MAGIC_CODE 0xCAFE

#include <uv.h>
#include "Transport/AbstractSocket.hpp"

class StdIOSocket : public AbstractSocket {

public:
  using OnCloseCallback = std::function<void()>;

  StdIOSocket(uv_loop_t* loop, std::shared_ptr<AbstractFormat> format);

  bool send(const std::vector<uint8_t>& data) override;
  void poll(OnReceivedCallback on_received, OnErrorCallback on_error) override;

  void on_close(OnCloseCallback on_close);
  void close();

protected:
  static void on_poll(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
  std::vector<uint8_t> generate_header(const std::vector<uint8_t>& data);

private:
  static void allocate_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
  bool receive(const uint8_t* data, size_t length);
  void clear();

  std::vector<uint8_t> m_header;
  std::vector<uint8_t> m_payload;
  size_t m_header_length;
  size_t m_payload_length;

  OnCloseCallback m_on_close_callback;

  std::unique_ptr<uv_pipe_t> m_poller;

};

#endif //BABLE_STDIOSOCKET_HPP
