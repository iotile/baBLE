#include "HCISocket.hpp"

using namespace std;
using namespace uvw;

Flags<PollHandle::Event> HCISocket::readable_flag = Flags<PollHandle::Event>(PollHandle::Event::READABLE);
Flags<PollHandle::Event> HCISocket::writable_flag = Flags<PollHandle::Event>(PollHandle::Event::WRITABLE);

HCISocket::HCISocket(shared_ptr<HCIFormat> format, uint16_t controller_id)
    : AbstractSocket(move(format)) {
  m_controller_id = controller_id;
  m_writable = true;

  m_socket = socket(AF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC | SOCK_NONBLOCK, BTPROTO_HCI);
  if (m_socket < 0) {
    throw Exceptions::SocketException("Error while creating the HCI socket: " + string(strerror(errno)));
  }

  if(!bind_socket()) {
    throw Exceptions::SocketException("Error while binding the HCI socket: " + string(strerror(errno)));
  }
}

bool HCISocket::bind_socket() {
  struct Format::HCI::sockaddr_hci addr {
      AF_BLUETOOTH,
      m_controller_id,
      HCI_CHANNEL_RAW
  };

  return bind(m_socket, (struct sockaddr*) &addr, sizeof(addr)) == 0;
}

bool HCISocket::send(const vector<uint8_t>& data) {
  if (!m_writable) {
    LOG.debug("Already sending a message. Queuing...", "HCISocket");
    m_send_queue.push(data);
    return false;

  } else {
    set_writable(false);

    LOG.info("Sending data...", "HCI socket");
    LOG.debug(data, "HCI socket");
    if (write(m_socket, data.data(), data.size()) < 0) {
      LOG.error("Error while sending a message to MGMT socket: " + string(strerror(errno)), "HCISocket");
      throw Exceptions::SocketException("Error occured while sending the packet through HCI socket.");
    }
  }

  return true;
}

vector<uint8_t> HCISocket::receive() {
  uint8_t type_code = 0;
  ssize_t length_read;

  // TODO: test without MSG_PEEK
  length_read = recv(m_socket, &type_code, sizeof(type_code), MSG_PEEK);
  if (length_read < sizeof(type_code)) {
    LOG.error("Error while reading the type code: " + string(strerror(errno)), "HCISocket");
    throw Exceptions::SocketException("Can't read type code on the HCI socket.");
  }

  size_t header_length;
  switch(type_code) {
    case Format::HCI::Type::Command:
      header_length = Format::HCI::command_header_length;
      break;

    case Format::HCI::Type::AsyncData:
      header_length = Format::HCI::async_data_header_length;
      break;

    case Format::HCI::Type::Event:
      header_length = Format::HCI::event_header_length;
      break;

    default:
      throw Exceptions::WrongFormatException("Unknown type code received on the HCI socket.");
  }

  vector<uint8_t> header(header_length);

  // MSG_PEEK is used to not remove data in socket queue. Else, for unknown reason, all the data are consumed and we
  // can't read the payload afterwards...
  length_read = recv(m_socket, header.data(), header.size(), MSG_PEEK);

  if (length_read < header_length) {
    LOG.error("Error while reading the header: " + string(strerror(errno)), "HCISocket");
    throw Exceptions::SocketException("Can't read header on the HCI socket.");
  }

  size_t payload_length = m_format->extract_payload_length(header);

  vector<uint8_t> result(header_length + payload_length);
  length_read = read(m_socket, result.data(), result.size());

  if (length_read < header_length + payload_length) {
    LOG.error("Error while reading the payload: " + string(strerror(errno)), "HCISocket");
    throw Exceptions::SocketException("Can't read payload on the HCI socket.");
  }

  return result;
}

void HCISocket::poll(shared_ptr<uvw::Loop> loop, CallbackFunction on_received) {
  m_poller = loop->resource<uvw::PollHandle>(m_socket);

  m_poller->on<uvw::PollEvent>([this, on_received](const uvw::PollEvent& event, const uvw::PollHandle& handle){
    if (event.flags & uvw::PollHandle::Event::READABLE) {
      LOG.info("Reading data...", "HCISocket");
      vector<uint8_t> received_payload = receive();
      on_received(received_payload);
    } else if (event.flags & uvw::PollHandle::Event::WRITABLE) {
      set_writable(true);
      m_poller->start(HCISocket::readable_flag);
    }
  });

  m_poller->start(HCISocket::readable_flag | HCISocket::writable_flag);
}

void HCISocket::set_writable(bool is_writable) {
  if (m_writable != is_writable) {
    m_writable = is_writable;

    if (!m_writable) {
      m_poller->start(HCISocket::readable_flag | HCISocket::writable_flag);
    }
  }

  if (m_writable) {
    if (!m_send_queue.empty()) {
      send(m_send_queue.front());
      m_send_queue.pop();
    }
  }
}

HCISocket::~HCISocket() {
  close(m_socket);
  LOG.debug("HCI socket closed", "HCI socket");
};
