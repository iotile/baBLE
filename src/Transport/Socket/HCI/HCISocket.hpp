#ifndef BABLE_LINUX_HCISOCKET_HPP
#define BABLE_LINUX_HCISOCKET_HPP

#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <memory>
#include <queue>
#include <unordered_map>
#include <uvw.hpp>

#include "../../AbstractSocket.hpp"
#include "../../../Log/Log.hpp"
#include "../../../Format/HCI/HCIFormat.hpp"
#include "../../../Exceptions/Socket/SocketException.hpp"
#include "../../../Application/Packets/Events/DeviceConnected/DeviceConnected.hpp"
#include "../../../Application/Packets/Events/DeviceDisconnected/DeviceDisconnected.hpp"

class HCISocket : public AbstractSocket {

public:
  explicit HCISocket(std::shared_ptr<HCIFormat> format, uint16_t controller_id, const std::array<uint8_t, 6>& controller_address);

  bool send(const std::vector<uint8_t>& data) override;
  void poll(std::shared_ptr<uvw::Loop> loop, CallbackFunction on_received) override;

  void set_writable(bool is_writable);
  std::vector<uint8_t> receive();

  ~HCISocket() override;

private:
  static uvw::Flags<uvw::PollHandle::Event> readable_flag;
  static uvw::Flags<uvw::PollHandle::Event> writable_flag;

  bool bind_hci_socket();
  bool set_filter();
  void connect_l2cap_socket(uint16_t connection_handle, const std::array<uint8_t, 6>& device_address, uint8_t device_address_type);
  void disconnect_l2cap_socket(uint16_t connection_handle);

  std::array<uint8_t, 6> m_controller_address;
  uvw::OSSocketHandle::Type m_hci_socket;
  std::unordered_map<uint16_t, uvw::OSSocketHandle::Type> m_l2cap_sockets;

  std::shared_ptr<uvw::PollHandle> m_poller;
  std::queue<std::vector<uint8_t>> m_send_queue;
  bool m_writable;

};


#endif //BABLE_LINUX_HCISOCKET_HPP
