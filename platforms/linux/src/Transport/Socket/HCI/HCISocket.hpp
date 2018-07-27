#ifndef BABLE_HCISOCKET_HPP
#define BABLE_HCISOCKET_HPP

#include <queue>
#include <unordered_map>
#include <uv.h>
#include "Transport/Socket/Socket.hpp"
#include "Transport/AbstractSocket.hpp"
#include "Format/HCI/HCIFormat.hpp"

class HCISocket : public AbstractSocket {

public:
  static std::vector<std::shared_ptr<HCISocket>> create_all(uv_loop_t* loop, std::shared_ptr<HCIFormat> hci_format);

  explicit HCISocket(uv_loop_t* loop, std::shared_ptr<HCIFormat> format, uint16_t controller_id);
  explicit HCISocket(uv_loop_t* loop, std::shared_ptr<HCIFormat> format, uint16_t controller_id, std::shared_ptr<Socket> hci_socket);

  bool send(const std::vector<uint8_t>& data) override;
  void poll(OnReceivedCallback on_received, OnErrorCallback on_error) override;

  void connect_l2cap_socket(uint16_t connection_handle, const std::array<uint8_t, 6>& device_address, uint8_t device_address_type);
  void disconnect_l2cap_socket(uint16_t connection_handle);

  void set_gatt_table(const std::vector<Format::HCI::Service>& services, const std::vector<Format::HCI::Characteristic>& characteristics);
  std::vector<Format::HCI::Service> get_services() const;
  std::vector<Format::HCI::Characteristic> get_characteristics() const;

  void set_in_progress_packets(uint16_t connection_handle, uint16_t num_packets_processed);

  std::string get_controller_address();

  void close() override;
  ~HCISocket() override;

protected:
  static void on_poll(uv_poll_t* handle, int status, int events);
  void set_writable(bool is_writable);

  std::shared_ptr<Socket> m_hci_socket;

private:
  bool set_filters();
  bool get_controller_info();

  std::vector<uint8_t> receive();

  std::array<uint8_t, 6> m_controller_address{};
  uint16_t m_buffer_size;
  std::unordered_map<uint16_t, uint16_t> m_in_progress_packets;
  std::unordered_map<uint16_t, Socket> m_l2cap_sockets;

  std::unique_ptr<uv_poll_t> m_poller;

  std::queue<std::vector<uint8_t>> m_send_queue;
  bool m_writable;

  std::vector<Format::HCI::Service> m_services;
  std::vector<Format::HCI::Characteristic> m_characteristics;

};


#endif //BABLE_HCISOCKET_HPP
