#ifndef BABLE_LINUX_HCISOCKET_HPP
#define BABLE_LINUX_HCISOCKET_HPP

#include <queue>
#include <uvw.hpp>

#include "../../AbstractSocket.hpp"
#include "../../../Format/HCI/HCIFormat.hpp"

class HCISocket : public AbstractSocket {

public:
  static std::vector<std::shared_ptr<HCISocket>> create_all(std::shared_ptr<HCIFormat> hci_format);

  // TODO: keep bindings connection_handle <=> device_address (in main.cpp) ?
  explicit HCISocket(std::shared_ptr<HCIFormat> format, uint16_t controller_id);

  bool send(const std::vector<uint8_t>& data) override;
  void poll(std::shared_ptr<uvw::Loop> loop, OnReceivedCallback on_received, OnErrorCallback on_error) override;

  void set_writable(bool is_writable);
  std::vector<uint8_t> receive();

  void connect_l2cap_socket(uint16_t connection_handle, const std::array<uint8_t, 6>& device_address, uint8_t device_address_type);
  void disconnect_l2cap_socket(uint16_t connection_handle);

  ~HCISocket() override;

private:
  static uvw::Flags<uvw::PollHandle::Event> readable_flag;
  static uvw::Flags<uvw::PollHandle::Event> writable_flag;

  bool bind_hci_socket();
  bool set_filter();
  bool get_controller_address();

  std::array<uint8_t, 6> m_controller_address;
  uvw::OSSocketHandle::Type m_hci_socket;
  std::unordered_map<uint16_t, uvw::OSSocketHandle::Type> m_l2cap_sockets;

  std::shared_ptr<uvw::PollHandle> m_poller;
  std::queue<std::vector<uint8_t>> m_send_queue;
  bool m_writable;

};


#endif //BABLE_LINUX_HCISOCKET_HPP
