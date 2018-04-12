#include "MGMTSocket.hpp"

MGMTSocket::MGMTSocket() {
  m_writable = true;
  m_socket = socket(PF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC | SOCK_NONBLOCK, BTPROTO_HCI);
  if (m_socket < 0) {
    throw std::runtime_error("Error while creating the MGMT socket.");
  }

  if(!bind_socket()) {
    throw std::runtime_error("Error while binding the MGMT socket.");
  }
}

bool MGMTSocket::bind_socket() {
  struct sockaddr_hci addr{
      AF_BLUETOOTH,
      HCI_DEV_NONE,
      HCI_CHANNEL_CONTROL
  };

  return bind(m_socket, (struct sockaddr*) &addr, sizeof(addr)) == 0;
}

bool MGMTSocket::send(const Serializer& ser) {
  LOG.info("Sending data...", "MGMT socket");

  if (!m_writable) {
    LOG.debug("Already sending a message. Queuing...");
    m_send_queue.push(ser);
    return false;

  } else {
    m_writable = false;
    if (write(m_socket, ser.buffer(), ser.size()) < 0) {
      LOG.error("Error while sending a message to MGMT socket. Queuing...");
      m_send_queue.push(ser);
      return false;
    }
  }

  return true;
}

Deserializer MGMTSocket::receive() {
  LOG.info("Receiving data...", "MGMT socket");

  std::vector<uint8_t> payload(1024);
  Deserializer deser;

  ssize_t length = read(m_socket, payload.data(), payload.size());

  if (length > 0) {
    payload.resize(static_cast<unsigned long>(length));
    deser.import(payload);
  }

  return deser;
}

void MGMTSocket::set_writable() {
  m_writable = true;
  while (!m_send_queue.empty()) {
    send(m_send_queue.front());
    m_send_queue.pop();
  }
}

uvw::OSSocketHandle::Type& MGMTSocket::get_socket() {
  return m_socket;
}

MGMTSocket::~MGMTSocket() {
  close(m_socket);
};
