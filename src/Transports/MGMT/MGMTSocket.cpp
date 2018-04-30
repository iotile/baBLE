#include "MGMTSocket.hpp"

MGMTSocket::MGMTSocket(): AbstractSocket(get_header_size()) {
  m_socket = socket(PF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC | SOCK_NONBLOCK, BTPROTO_HCI);
  if (m_socket < 0) {
    throw std::runtime_error("Error while creating the MGMT socket.");
  }

  if(!bind_socket()) {
    throw std::runtime_error("Error while binding the MGMT socket.");
  }

  set_writable(true);
}

bool MGMTSocket::bind_socket() {
  struct sockaddr_hci addr{
      AF_BLUETOOTH,
      HCI_DEV_NONE,
      HCI_CHANNEL_CONTROL
  };

  return bind(m_socket, (struct sockaddr*) &addr, sizeof(addr)) == 0;
}

bool MGMTSocket::send(const std::vector<uint8_t>& data) {
  LOG.info("Sending data...", "MGMT socket");

  if (!m_writable) {
    LOG.debug("Already sending a message. Queuing...");
    m_send_queue.push(data);
    return false;

  } else {
    set_writable(false);
    if (write(m_socket, data.data(), data.size()) < 0) {
      LOG.error("Error while sending a message to MGMT socket.");
      throw std::runtime_error("Error occured while sending the packet through MGMT socket.");
    }
  }

  return true;
}

void MGMTSocket::poll(std::shared_ptr<uvw::Loop> loop, CallbackFunction on_received) {
  auto poller = loop->resource<uvw::PollHandle>(m_socket);

  poller->on<uvw::PollEvent>([this, &on_received](const uvw::PollEvent& event, const uvw::PollHandle& handle){
    if (event.flags & uvw::PollHandle::Event::READABLE) {
      LOG.debug("Readable data...", "MGMTSocket");
      std::vector<uint8_t> buffer = receive();

      if (!receive_with_header(buffer.data(), buffer.size())) {
        return;
      }
      on_received(m_received_payload);
      clear();
    } else if (event.flags & uvw::PollHandle::Event::WRITABLE) {
      set_writable(true);
    }
  });

  uvw::Flags<uvw::PollHandle::Event> readable = uvw::Flags<uvw::PollHandle::Event>(uvw::PollHandle::Event::READABLE);
  uvw::Flags<uvw::PollHandle::Event> writable = uvw::Flags<uvw::PollHandle::Event>(uvw::PollHandle::Event::WRITABLE);

  poller->start(readable | writable);
}

std::vector<uint8_t> MGMTSocket::receive() {
  LOG.info("Receiving data...", "MGMT socket");
  std::vector<uint8_t> buffer(1024);
  ssize_t length = read(m_socket, buffer.data(), buffer.size());

  if (length <= 0) {
    throw std::runtime_error("Nothing read on the socket.");
  }

  buffer.resize(static_cast<size_t>(length));
  buffer.shrink_to_fit();

  return buffer;
}

void MGMTSocket::set_writable(bool is_writable) {
  m_writable = is_writable;

  if (m_writable) {
    while (!m_send_queue.empty()) {
      send(m_send_queue.front());
      m_send_queue.pop();
    }
  }
}

MGMTSocket::~MGMTSocket() {
  close(m_socket);
};
