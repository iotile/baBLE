#ifndef BABLE_LINUX_MGMTSOCKET_HPP
#define BABLE_LINUX_MGMTSOCKET_HPP

#include <queue>
#include <uvw.hpp>

#include "../../AbstractSocket.hpp"
#include "../../../Format/MGMT/MGMTFormat.hpp"

class MGMTSocket : public AbstractSocket {

public:
  explicit MGMTSocket(std::shared_ptr<MGMTFormat> format);

  bool send(const std::vector<uint8_t>& data) override;
  void poll(std::shared_ptr<uvw::Loop> loop, OnReceivedCallback on_received, OnErrorCallback on_error) override;

  void sync_send(const std::vector<uint8_t>& data);
  std::vector<uint8_t> sync_receive();

  void set_writable(bool is_writable);

  ~MGMTSocket() override;

private:
  static uvw::Flags<uvw::PollHandle::Event> readable_flag;
  static uvw::Flags<uvw::PollHandle::Event> writable_flag;

  bool bind_socket();
  void set_blocking(bool state);
  std::vector<uint8_t> receive();

  size_t m_header_length;
  uvw::OSSocketHandle::Type m_socket;
  std::shared_ptr<uvw::PollHandle> m_poller;
  std::queue<std::vector<uint8_t>> m_send_queue;
  bool m_writable;

};

#endif //BABLE_LINUX_MGMTSOCKET_HPP
