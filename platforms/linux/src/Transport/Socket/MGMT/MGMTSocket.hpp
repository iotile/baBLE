#ifndef BABLE_LINUX_MGMTSOCKET_HPP
#define BABLE_LINUX_MGMTSOCKET_HPP

#include <queue>
#include <uv.h>
#include "Transport/Socket/Socket.hpp"
#include "Transport/AbstractSocket.hpp"
#include "Format/MGMT/MGMTFormat.hpp"

class MGMTSocket : public AbstractSocket {

public:
  explicit MGMTSocket(uv_loop_t* loop, std::shared_ptr<MGMTFormat> format);
  explicit MGMTSocket(uv_loop_t* loop, std::shared_ptr<MGMTFormat> format, std::shared_ptr<Socket> socket);

  bool send(const std::vector<uint8_t>& data) override;
  void poll(OnReceivedCallback on_received, OnErrorCallback on_error) override;

  ~MGMTSocket() override;

protected:
  static void on_poll(uv_poll_t* handle, int status, int events);
  void set_writable(bool is_writable);

  std::shared_ptr<Socket> m_socket;

private:
  std::vector<uint8_t> receive();

  size_t m_header_length;

  std::unique_ptr<uv_poll_t> m_poller;

  std::queue<std::vector<uint8_t>> m_send_queue;
  bool m_writable;

};

#endif //BABLE_LINUX_MGMTSOCKET_HPP
