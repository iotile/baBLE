#ifndef BABLE_MOCK_HCI_SOCKET_HPP
#define BABLE_MOCK_HCI_SOCKET_HPP

#include "Transport/Socket/HCI/HCISocket.hpp"
#include "mock_socket.hpp"

class MockHCISocket : public HCISocket {

public:
  MockHCISocket(uv_loop_t* loop, std::shared_ptr<HCIFormat> format, uint16_t controller_id, const std::array<uint8_t, 6>& bd_addr)
      : HCISocket(loop, move(format), controller_id, std::make_shared<MockSocket>(bd_addr)) {
    m_mock_socket = std::static_pointer_cast<MockSocket>(m_hci_socket);
    m_poll = std::make_unique<uv_poll_t>();
    m_poll->data = this;
  };

  void poll(OnReceivedCallback on_received, OnErrorCallback on_error) override {
    m_on_received = on_received;
    m_on_error = on_error;
  }

  void simulate_read(const std::vector<uint8_t>& data, bool writable = true) {
    m_mock_socket->simulate_read(data);

    set_writable(writable);
    on_poll(m_poll.get(), 0, UV_READABLE);
  };

  std::shared_ptr<MockSocket> get_raw() {
    return m_mock_socket;
  }

private:
  std::unique_ptr<uv_poll_t> m_poll;
  std::shared_ptr<MockSocket> m_mock_socket;

};

#endif //BABLE_MOCK_HCI_SOCKET_HPP
