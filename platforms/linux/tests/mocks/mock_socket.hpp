#ifndef BABLE_MOCK_SOCKET_HPP
#define BABLE_MOCK_SOCKET_HPP

#include "Format/HCI/constants.hpp"
#include "Transport/Socket/Socket.hpp"

class MockSocket : public Socket {

public:
  MockSocket(): Socket() {
    m_num_read = 0;
    m_num_write = 0;

    m_open = true;
  };

  explicit MockSocket(const std::array<uint8_t, 6>& bd_addr): MockSocket() {
    m_address = bd_addr;
  };

  void bind(uint16_t device, uint16_t channel) override {
    m_binded = true;
  };

  void bind(const std::array<uint8_t, 6>& address, uint8_t address_type, uint16_t channel) override {
    m_binded = true;
  };

  void write(const std::vector<uint8_t>& data) override {
    m_num_write++;
    m_write_buffers.push_back(data);
  };

  ssize_t read(std::vector<uint8_t>& data, bool peek) override {
    m_num_read++;
    data.assign(m_read_buffer.begin(), m_read_buffer.end());
    return m_read_buffer.size();
  };

  void set_option(int level, int name, const void *val, socklen_t len) override {
    m_option_set = true;
  };

  void ioctl(uint64_t request, void* param) override {
    if (request == HCIGETDEVINFO) {
      auto dev = static_cast<struct Format::HCI::hci_dev_info*>(param);
      bdaddr_t bdaddr{};
      std::copy(m_address.begin(), m_address.end(), std::begin(bdaddr.b));
      dev->bdaddr = bdaddr;
    };
  };

  void connect(const std::array<uint8_t, 6>& address, uint8_t address_type, uint16_t channel) override {
    m_connected = true;
  };

  void close() override {
    m_open = false;
  };

  const std::vector<uint8_t>& get_write_buffer(size_t index) {
    return m_write_buffers.at(index);
  };

  size_t get_num_write_buffers() {
    return m_write_buffers.size();
  };

  void reset_write_buffers() {
    m_write_buffers.clear();
  };

  void simulate_read(const std::vector<uint8_t>& data) {
    m_read_buffer.assign(data.begin(), data.end());
  };

  bool has_read(int expected_num = -1) {
    if (expected_num < 0) {
      return m_num_read > 0;
    } else {
      return m_num_read == expected_num;
    }
  };

  bool has_written(int expected_num = -1) {
    if (expected_num < 0) {
      return m_num_write > 0;
    } else {
      return m_num_write == expected_num;
    }  };

private:
  std::array<uint8_t, 6> m_address{};
  std::vector<std::vector<uint8_t>> m_write_buffers;
  std::vector<uint8_t> m_read_buffer;
  int m_num_read;
  int m_num_write;

};

#endif //BABLE_MOCK_SOCKET_HPP
