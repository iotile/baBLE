#ifndef BABLE_LINUX_MGMTSOCKET_HPP
#define BABLE_LINUX_MGMTSOCKET_HPP

#include <queue>
#include <uv.h>

#include "../../AbstractSocket.hpp"
#include "../../../Format/MGMT/MGMTFormat.hpp"

class MGMTSocket : public AbstractSocket {

public:
  explicit MGMTSocket(uv_loop_t* loop, std::shared_ptr<MGMTFormat> format);

  bool send(const std::vector<uint8_t>& data) override;
  void poll(OnReceivedCallback on_received, OnErrorCallback on_error) override;

  ~MGMTSocket() override;

private:
  static void on_poll(uv_poll_t* handle, int status, int events);

  bool bind_socket();

  std::vector<uint8_t> receive();
  void set_writable(bool is_writable);

  size_t m_header_length;
  uv_os_sock_t m_socket;

  std::unique_ptr<uv_poll_t> m_poller;

  std::queue<std::vector<uint8_t>> m_send_queue;
  bool m_writable;

};

#endif //BABLE_LINUX_MGMTSOCKET_HPP
