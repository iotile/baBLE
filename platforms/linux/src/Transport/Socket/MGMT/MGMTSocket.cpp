#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "MGMTSocket.hpp"
#include "Log/Log.hpp"
#include "Exceptions/BaBLEException.hpp"

using namespace std;

MGMTSocket::MGMTSocket(uv_loop_t* loop, shared_ptr<MGMTFormat> format)
    : MGMTSocket(loop, move(format), make_shared<Socket>(AF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC | SOCK_NONBLOCK, BTPROTO_HCI))
{}

MGMTSocket::MGMTSocket(uv_loop_t* loop, shared_ptr<MGMTFormat> format, shared_ptr<Socket> socket)
    : AbstractSocket(move(format)),
      m_socket(move(socket)) {
  m_header_length = m_format->get_header_length(0);
  m_writable = true;

  LOG.debug("Binding MGMT socket...", "MGMTSocket");
  m_socket->bind(NON_CONTROLLER_ID, HCI_CHANNEL_CONTROL);

  LOG.debug("Setting up poller on MGMT socket...", "MGMTSocket");
  m_poller = make_unique<uv_poll_t>();
  m_poller->data = this;
  uv_poll_init_socket(loop, m_poller.get(), static_cast<uv_os_sock_t>(m_socket->get_raw()));

  LOG.debug("MGMT socket created", "MGMTSocket");
}

bool MGMTSocket::send(const vector<uint8_t>& data) {
  if (!m_writable) {
    LOG.debug("Already sending a message. Queuing...", "MGMTSocket");
    m_send_queue.push(data);
    return false;

  } else {
    set_writable(false);

//    LOG.debug("Sending data...", "MGMTSocket");
//    LOG.debug(data, "MGMTSocket");
    m_socket->write(data);
  }

  return true;
}

vector<uint8_t> MGMTSocket::receive() {
  // MSG_PEEK is used to not remove data in socket queue. Else, for unknown reason, all the data are consumed and we
  // can't read the payload afterwards...
  vector<uint8_t> header(m_header_length);
  m_socket->read(header, true);

  size_t payload_length = m_format->extract_payload_length(header);

  vector<uint8_t> result(m_header_length + payload_length);
  m_socket->read(result, false);

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
      vector<uint8_t> received_payload = mgmt_socket->receive();
//      LOG.debug("Data received", "MGMTSocket");
//      LOG.debug(received_payload, "MGMTSocket");
      mgmt_socket->m_on_received(received_payload, mgmt_socket);

    } else if (events & UV_WRITABLE) {
      mgmt_socket->set_writable(true);
    }

  } catch (const Exceptions::BaBLEException& err) {
    mgmt_socket->m_on_error(err);
  }
}

void MGMTSocket::poll(OnReceivedCallback on_received, OnErrorCallback on_error) {
  m_on_received = move(on_received);
  m_on_error = move(on_error);

  uv_poll_start(m_poller.get(), UV_READABLE | UV_WRITABLE, on_poll);
  m_poll_started = true;
}

void MGMTSocket::set_writable(bool is_writable) {
  m_writable = is_writable;

  if (m_poll_started) {
    if (m_writable) {
      uv_poll_start(m_poller.get(), UV_READABLE, on_poll);
    } else {
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

void MGMTSocket::close() {
  m_socket->close();
  LOG.debug("MGMT socket closed", "MGMTSocket");
}

MGMTSocket::~MGMTSocket() {
  close();
}
