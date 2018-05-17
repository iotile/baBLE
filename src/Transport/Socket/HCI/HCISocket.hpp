#ifndef BABLE_LINUX_HCISOCKET_HPP
#define BABLE_LINUX_HCISOCKET_HPP

#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <memory>
#include <queue>
#include <uvw.hpp>

#include "../../AbstractSocket.hpp"
#include "../../../Log/Log.hpp"
#include "../../../Format/HCI/HCIFormat.hpp"
#include "../../../Exceptions/Socket/SocketException.hpp"

class HCISocket : public AbstractSocket {

public:
  explicit HCISocket(std::shared_ptr<HCIFormat> format, uint16_t controller_id);

  bool send(const std::vector<uint8_t>& data) override;
  void poll(std::shared_ptr<uvw::Loop> loop, CallbackFunction on_received) override;

  void set_writable(bool is_writable);
  std::vector<uint8_t> receive();

  ~HCISocket() override;

private:
  static uvw::Flags<uvw::PollHandle::Event> readable_flag;
  static uvw::Flags<uvw::PollHandle::Event> writable_flag;

  bool bind_hci_socket();
  bool bind_l2cap_socket();

  uvw::OSSocketHandle::Type m_hci_socket;
  uvw::OSSocketHandle::Type m_l2cap_socket;

  std::shared_ptr<uvw::PollHandle> m_poller;
  std::queue<std::vector<uint8_t>> m_send_queue;
  bool m_writable;

};


#endif //BABLE_LINUX_HCISOCKET_HPP
