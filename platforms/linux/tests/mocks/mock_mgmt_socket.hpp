#ifndef BABLE_MOCK_MGMT_SOCKET_HPP
#define BABLE_MOCK_MGMT_SOCKET_HPP

#include "Transport/Socket/MGMT/MGMTSocket.hpp"

class MockMGMTSocket : public MGMTSocket {

public:
  MockMGMTSocket(uv_loop_t* loop, std::shared_ptr<MGMTFormat> format)
      : MGMTSocket(loop, move(format), std::make_shared<MockSocket>()) {
    m_mock_socket = std::static_pointer_cast<MockSocket>(m_socket);
    m_poll = std::make_unique<uv_poll_t>();
    m_poll->data = this;
  };

  void poll(OnReceivedCallback on_received, OnErrorCallback on_error) override {
    m_on_received = on_received;
    m_on_error = on_error;
  };

  void simulate_read(const std::vector<uint8_t>& data, bool writable = true) {
    m_mock_socket->simulate_read(data);

    set_writable(writable);
    on_poll(m_poll.get(), 0, UV_READABLE);
  };

  std::shared_ptr<MockSocket> get_raw() {
    return m_mock_socket;
  };

private:
  std::unique_ptr<uv_poll_t> m_poll;
  std::shared_ptr<MockSocket> m_mock_socket;

};

#endif //BABLE_MOCK_MGMT_SOCKET_HPP
