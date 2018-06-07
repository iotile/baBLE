#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include "MGMTSocket.hpp"
#include "../../../Exceptions/Socket/SocketException.hpp"
#include "../../../Log/Log.hpp"

using namespace std;
using namespace uvw;

Flags<PollHandle::Event> MGMTSocket::readable_flag = Flags<PollHandle::Event>(PollHandle::Event::READABLE);
Flags<PollHandle::Event> MGMTSocket::writable_flag = Flags<PollHandle::Event>(PollHandle::Event::WRITABLE);

MGMTSocket::MGMTSocket(shared_ptr<uvw::Loop>& loop, shared_ptr<MGMTFormat> format)
    : AbstractSocket(move(format)) {
  m_header_length = m_format->get_header_length(0);
  m_writable = true;

  m_socket = socket(AF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC | SOCK_NONBLOCK, BTPROTO_HCI);
  if (m_socket < 0) {
    throw Exceptions::SocketException("Error while creating the MGMT socket.");
  }

  if(!bind_socket()) {
    throw Exceptions::SocketException("Error while binding the MGMT socket.");
  }

  m_poller = loop->resource<PollHandle>(m_socket);
}

bool MGMTSocket::bind_socket() {
  struct Format::HCI::sockaddr_hci addr {
      AF_BLUETOOTH,
      NON_CONTROLLER_ID,
      HCI_CHANNEL_CONTROL
  };

  return bind(m_socket, (struct sockaddr*) &addr, sizeof(addr)) == 0;
}

void MGMTSocket::sync_send(const vector<uint8_t>& data) {
  // Used to do a blocking write on MGMT socket
  set_blocking(true);
  ssize_t length_written = write(m_socket, data.data(), data.size());
  set_blocking(false);

  if (length_written < 0) {
    LOG.error("Error while synchronously sending a message to MGMT socket: " + string(strerror(errno)), "MGMTSocket");
    throw Exceptions::SocketException("Error occured while synchronously sending the packet through MGMT socket.");
  }
}

bool MGMTSocket::send(const vector<uint8_t>& data) {
  if (!m_writable) {
    LOG.debug("Already sending a message. Queuing...", "MGMTSocket");
    m_send_queue.push(data);
    return false;

  } else {
    set_writable(false);

    LOG.debug("Sending data...", "MGMT socket");
    LOG.debug(data, "MGMT socket");
    if (write(m_socket, data.data(), data.size()) < 0) {
      LOG.error("Error while sending a message to MGMT socket: " + string(strerror(errno)), "MGMTSocket");
      throw Exceptions::SocketException("Error occured while sending the packet through MGMT socket.");
    }
  }

  return true;
}

void MGMTSocket::set_blocking(bool state) {
  /* Save the current flags */
  int flags = fcntl(m_socket, F_GETFL, 0);
  if (flags == -1) {
    throw Exceptions::SocketException("Error while getting the MGMT socket flags.");
  }

  if (state) {
    flags &= ~O_NONBLOCK;
  } else {
    flags |= O_NONBLOCK;
  }

  if (fcntl(m_socket, F_SETFL, flags) == -1) {
    throw Exceptions::SocketException("Error while setting the blocking flag of the MGMT socket.");
  }
}

vector<uint8_t> MGMTSocket::sync_receive() {
  // Used to do a blocking read on MGMT socket

  set_blocking(true);
  vector<uint8_t> data = receive();
  set_blocking(false);

  return data;
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

void MGMTSocket::poll(OnReceivedCallback on_received, OnErrorCallback on_error) {
  m_poller->on<PollEvent>([this, on_received, on_error](const PollEvent& event, const PollHandle& handle){
    try {
      if (event.flags & PollHandle::Event::READABLE) {
        LOG.debug("Reading data...", "MGMTSocket");
        vector<uint8_t> received_payload = receive();
        on_received(received_payload, m_format);
      } else if (event.flags & PollHandle::Event::WRITABLE) {
        set_writable(true);
        m_poller->start(MGMTSocket::readable_flag);
      }

    } catch (const Exceptions::AbstractException& err) {
      on_error(err);
    }
  });

  m_poller->start(MGMTSocket::readable_flag | MGMTSocket::writable_flag);
}

void MGMTSocket::set_writable(bool is_writable) {
  if (m_writable != is_writable) {
    m_writable = is_writable;

    if (!m_writable) {
      m_poller->start(MGMTSocket::readable_flag | MGMTSocket::writable_flag);
    }
  }

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
