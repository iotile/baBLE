#include "MGMTSocket.hpp"

using namespace std;

MGMTSocket::MGMTSocket(shared_ptr<MGMTFormat> format)
    : AbstractSocket(move(format)) {
  m_header_length = Format::MGMT::header_length;

  m_socket = socket(PF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC | SOCK_NONBLOCK, BTPROTO_HCI);
  if (m_socket < 0) {
    throw Exceptions::SocketException("Error while creating the MGMT socket.");
  }

  if(!bind_socket()) {
    throw Exceptions::SocketException("Error while binding the MGMT socket.");
  }

  set_writable(true);
}

bool MGMTSocket::bind_socket() {
  struct sockaddr_hci addr{
      AF_BLUETOOTH,
      Format::MGMT::non_controller_id,
      HCI_CHANNEL_CONTROL
  };

  return bind(m_socket, (struct sockaddr*) &addr, sizeof(addr)) == 0;
}

bool MGMTSocket::send(const vector<uint8_t>& data) {
  if (!m_writable) {
    LOG.debug("Already sending a message. Queuing...");
    m_send_queue.push(data);
    return false;

  } else {
    set_writable(false);
    LOG.info("Sending data...", "MGMT socket");
    LOG.debug(data, "MGMT socket");
    if (write(m_socket, data.data(), data.size()) < 0) {
      LOG.error("Error while sending a message to MGMT socket: " + string(strerror(errno)), "MGMTSocket");
      throw Exceptions::SocketException("Error occured while sending the packet through MGMT socket.");
    }
  }

  return true;
}

vector<uint8_t> MGMTSocket::receive() {
  vector<uint8_t> header(m_header_length);

  // MSG_PEEK is used to not remove data in socket queue. Else, for unknown reason, all the data are consumed and we
  // can't read the payload afterwards...
  ssize_t length_read = recv(m_socket, header.data(), header.size(), MSG_PEEK);

  if (length_read < m_header_length) {
    LOG.error("Error while reading the header: " + string(strerror(errno)), "MGMTSocket");
    throw Exceptions::SocketException("Can't read header on the MGMT socket.");
  }

  size_t payload_length = m_format->extract_payload_length(header);

  vector<uint8_t> result(m_header_length + payload_length);
  length_read = read(m_socket, result.data(), result.size());

  if (length_read < m_header_length + payload_length) {
    LOG.error("Error while reading the payload: " + string(strerror(errno)), "MGMTSocket");
    throw Exceptions::SocketException("Can't read payload on the MGMT socket.");
  }

  return result;
}

void MGMTSocket::poll(shared_ptr<uvw::Loop> loop, CallbackFunction on_received) {
  auto poller = loop->resource<uvw::PollHandle>(m_socket);

  poller->on<uvw::PollEvent>([this, on_received](const uvw::PollEvent& event, const uvw::PollHandle& handle){
    if (event.flags & uvw::PollHandle::Event::READABLE) {
      LOG.info("Reading data...", "MGMTSocket");
      vector<uint8_t> received_payload = receive();
      on_received(received_payload);
    } else if (event.flags & uvw::PollHandle::Event::WRITABLE) {
      set_writable(true);
    }
  });

  uvw::Flags<uvw::PollHandle::Event> readable = uvw::Flags<uvw::PollHandle::Event>(uvw::PollHandle::Event::READABLE);
  uvw::Flags<uvw::PollHandle::Event> writable = uvw::Flags<uvw::PollHandle::Event>(uvw::PollHandle::Event::WRITABLE);

  poller->start(readable | writable);
}

void MGMTSocket::set_writable(bool is_writable) {
  m_writable = is_writable;

  if (m_writable) {
    if (!m_send_queue.empty()) {
      send(m_send_queue.front());
      m_send_queue.pop();
    }
  }
}

MGMTSocket::~MGMTSocket() {
  close(m_socket);
  LOG.debug("MGMT socket closed", "MGMT socket");
};
