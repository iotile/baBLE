#ifndef BABLE_LINUX_HCISOCKET_HPP
#define BABLE_LINUX_HCISOCKET_HPP

#include <queue>
#include <unordered_map>
#include <uv.h>
#include "Transport/Socket/Socket.hpp"
#include "Transport/AbstractSocket.hpp"
#include "Format/HCI/HCIFormat.hpp"
#include "utils/string_formats.hpp"

class HCISocket : public AbstractSocket {

public:
  static std::vector<std::shared_ptr<HCISocket>> create_all(uv_loop_t* loop, std::shared_ptr<HCIFormat> hci_format);

  explicit HCISocket(uv_loop_t* loop, std::shared_ptr<HCIFormat> format, uint16_t controller_id);
  explicit HCISocket(uv_loop_t* loop, std::shared_ptr<HCIFormat> format, uint16_t controller_id, std::shared_ptr<Socket> hci_socket);

  bool send(const std::vector<uint8_t>& data) override;
  void poll(OnReceivedCallback on_received, OnErrorCallback on_error) override;

  void connect_l2cap_socket(uint16_t connection_handle, const std::array<uint8_t, 6>& device_address, uint8_t device_address_type);
  void disconnect_l2cap_socket(uint16_t connection_handle);

  std::string get_controller_address() {
    return Utils::format_bd_address(m_controller_address);
  };

  ~HCISocket() override;

protected:
  static void on_poll(uv_poll_t* handle, int status, int events);
  void set_writable(bool is_writable);

  std::shared_ptr<Socket> m_hci_socket;

private:
  bool set_filters();
  bool find_controller_address();

  std::vector<uint8_t> receive();

  std::array<uint8_t, 6> m_controller_address{};
  std::unordered_map<uint16_t, Socket> m_l2cap_sockets;

  std::unique_ptr<uv_poll_t> m_poller;

  std::queue<std::vector<uint8_t>> m_send_queue;
  bool m_writable;

};


#endif //BABLE_LINUX_HCISOCKET_HPP
