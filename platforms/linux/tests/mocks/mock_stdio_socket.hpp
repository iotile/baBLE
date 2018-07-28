#ifndef BABLE_MOCK_STDIO_SOCKET_HPP
#define BABLE_MOCK_STDIO_SOCKET_HPP

#include "Transport/Socket/StdIO/StdIOSocket.hpp"

class MockStdIOSocket : public StdIOSocket {

public:
  MockStdIOSocket(uv_loop_t* loop, std::shared_ptr<AbstractFormat> format): StdIOSocket(loop, move(format)) {
    m_stream = std::make_unique<uv_stream_t>();
    m_stream->data = this;
  };

  bool send(const std::vector<uint8_t>& data, uint16_t connection_handle) override {
    m_buffers.push_back(data);
  };

  void poll(OnReceivedCallback on_received, OnErrorCallback on_error) override {
    m_on_received = on_received;
    m_on_error = on_error;
  }

  void simulate_read(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> simulated_received_data = generate_header(data);
    simulated_received_data.insert(simulated_received_data.end(), data.begin(), data.end());

    std::unique_ptr<uv_buf_t> buf = std::make_unique<uv_buf_t>();
    buf->base = reinterpret_cast<char*>(simulated_received_data.data());

    MockStdIOSocket::on_poll(m_stream.get(), simulated_received_data.size(), buf.get());
  };

  const std::vector<uint8_t>& get_write_buffer(size_t index) {
    return m_buffers.at(index);
  };

  size_t get_num_write_buffers() {
    return m_buffers.size();
  };

  void reset_write_buffers() {
    m_buffers.clear();
  };

private:
  std::unique_ptr<uv_stream_t> m_stream;
  std::vector<std::vector<uint8_t>> m_buffers;

};

#endif //BABLE_MOCK_STDIO_SOCKET_HPP
