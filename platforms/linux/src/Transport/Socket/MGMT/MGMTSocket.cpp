#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "MGMTSocket.hpp"
#include "../../../Log/Log.hpp"
#include "../../../Exceptions/BaBLEException.hpp"

using namespace std;

MGMTSocket::MGMTSocket(uv_loop_t* loop, shared_ptr<MGMTFormat> format)
    : AbstractSocket(move(format)) {
  m_header_length = m_format->get_header_length(0);
  m_writable = true;

  m_socket = socket(AF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC | SOCK_NONBLOCK, BTPROTO_HCI);
  if (m_socket < 0) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::SocketError,
        "Error while creating the MGMT socket: " + string(strerror(errno))
    );
  }

  if(!bind_socket()) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::SocketError,
        "Error while binding the MGMT socket: " + string(strerror(errno))
    );
  }

  m_poller = make_unique<uv_poll_t>();
  m_poller->data = this;
  uv_poll_init_socket(loop, m_poller.get(), m_socket);
}

bool MGMTSocket::bind_socket() {
  struct Format::HCI::sockaddr_hci addr {
      AF_BLUETOOTH,
      NON_CONTROLLER_ID,
      HCI_CHANNEL_CONTROL
  };

  return bind(m_socket, (struct sockaddr*) &addr, sizeof(addr)) == 0;
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
      throw Exceptions::BaBLEException(
          BaBLE::StatusCode::SocketError,
          "Error occured while sending the packet through MGMT socket: " + string(strerror(errno))
      );
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
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::SocketError,
        "Error while reading the MGMT header: " + string(strerror(errno))
    );
  }

  size_t payload_length = m_format->extract_payload_length(header);

  vector<uint8_t> result(m_header_length + payload_length);
  length_read = read(m_socket, result.data(), result.size());

  if (length_read < m_header_length + payload_length) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::SocketError,
        "Error while reading the MGMT payload: " + string(strerror(errno))
    );
  }

  return result;
}

void MGMTSocket::on_poll(uv_poll_t* handle, int status, int events) {
  if (status < 0) {
    LOG.error("Error while polling MGMT socket: " + string(uv_err_name(status)), "MGMTSocket");
    return;
  }

  auto mgmt_socket = static_cast<MGMTSocket*>(handle->data);

  try {
    if (events & UV_READABLE) {
      LOG.debug("Reading data...", "MGMTSocket");
      vector<uint8_t> received_payload = mgmt_socket->receive();
      mgmt_socket->m_on_received(received_payload, mgmt_socket->m_format);

    } else if (events & UV_WRITABLE) {
      mgmt_socket->set_writable(true);
      uv_poll_start(mgmt_socket->m_poller.get(), UV_READABLE, mgmt_socket->on_poll);
    }

  } catch (const Exceptions::BaBLEException& err) {
    mgmt_socket->m_on_error(err);
  }
}

void MGMTSocket::poll(OnReceivedCallback on_received, OnErrorCallback on_error) {
  m_on_received = move(on_received);
  m_on_error = move(on_error);

  uv_poll_start(m_poller.get(), UV_READABLE | UV_WRITABLE, on_poll);
}

void MGMTSocket::set_writable(bool is_writable) {
  if (m_writable != is_writable) {
    m_writable = is_writable;

    if (!m_writable) {
      uv_poll_start(m_poller.get(), UV_READABLE | UV_WRITABLE, on_poll);
    }
  }

  if (m_writable) {
    if (!m_send_queue.empty()) {
      send(reinterpret_cast<const vector<uint8_t>&>(m_send_queue.front()));
      m_send_queue.pop();
    }
  }
}

MGMTSocket::~MGMTSocket() {
  close(m_socket);
  LOG.debug("MGMT socket closed", "MGMT socket");
};
