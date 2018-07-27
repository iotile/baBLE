#include "StdIOSocket.hpp"
#include "Log/Log.hpp"
#include "Exceptions/BaBLEException.hpp"

using namespace std;

StdIOSocket::StdIOSocket(uv_loop_t* loop, shared_ptr<AbstractFormat> format)
: AbstractSocket(move(format)) {
  m_header_length = 4;
  m_payload_length = 0;
  m_header.reserve(m_header_length);

  m_poller = make_unique<uv_pipe_t>();
  m_poller->data = this;
  uv_pipe_init(loop, m_poller.get(), false);
  uv_pipe_open(m_poller.get(), fileno(stdin));

  m_on_close_callback = []() {};
}

bool StdIOSocket::send(const vector<uint8_t>& data) {
  vector<uint8_t> header = generate_header(data);

  fwrite(header.data(), sizeof(uint8_t), header.size(), stdout);
  fwrite(data.data(), sizeof(uint8_t), data.size(), stdout);

  fflush(stdout);
  return true;
}

void StdIOSocket::allocate_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  auto size = static_cast<unsigned int>(suggested_size);
  *buf = uv_buf_init(new char[size], size);
}

void StdIOSocket::on_poll(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  auto stdio_socket = static_cast<StdIOSocket*>(stream->data);

  if (nread == UV_EOF) {
    LOG.debug("StdIO socket closed...", "StdIOSocket");
    stdio_socket->close();

  } else if (nread < 0) {
    auto error_code = static_cast<int>(nread);
    LOG.error("Error while polling input: " + string(uv_err_name(error_code)));

  } else if (nread > 0) {
    auto remaining_data_length = static_cast<size_t>(nread);
    auto remaining_data = reinterpret_cast<uint8_t*>(buf->base);

    while (remaining_data_length > 0) {
      try {
//        LOG.debug("Remaining data: " + to_string(remaining_data_length), "StdIOSocket");
        if (!stdio_socket->receive(remaining_data, remaining_data_length)) {
          return;
        }

        size_t consumed_data_length = stdio_socket->m_header.size() + stdio_socket->m_payload.size();
        if (consumed_data_length > remaining_data_length) {
          LOG.error("Wrong value of data length consumed (consumed data=" + to_string(consumed_data_length) +
              ", remaining data=" + to_string(remaining_data_length));
          throw Exceptions::BaBLEException(BaBLE::StatusCode::SocketError, "Error while receiving data on StdIO socket. Stream ignored");
        }

        remaining_data_length -= consumed_data_length;
        remaining_data += consumed_data_length;

        stdio_socket->m_on_received(stdio_socket->m_payload, stdio_socket);
      } catch (const Exceptions::BaBLEException& err) {
        stdio_socket->m_on_error(err);
      }

      stdio_socket->clear(); // To clean variables, before receiving other data in the next loop
    }
  }
}

void StdIOSocket::poll(OnReceivedCallback on_received, OnErrorCallback on_error) {
  m_on_received = move(on_received);
  m_on_error = move(on_error);

  uv_read_start((uv_stream_t*)m_poller.get(), allocate_buffer, on_poll);
}

void StdIOSocket::on_close(OnCloseCallback on_close) {
  m_on_close_callback = move(on_close);
}

void StdIOSocket::close() {
  m_on_close_callback();
  LOG.debug("StdIOSocket closed", "StdIOSocket");
}

vector<uint8_t> StdIOSocket::generate_header(const vector<uint8_t>& data) {
  auto payload_length = static_cast<uint16_t>(data.size());

  vector<uint8_t> header;
  header.reserve(m_header_length);

  header.push_back(static_cast<uint8_t>(MAGIC_CODE >> 8));
  header.push_back(static_cast<uint8_t>(MAGIC_CODE & 0x00FF));

  // Use little endian
  header.push_back(static_cast<uint8_t>(payload_length & 0x00FF));
  header.push_back(static_cast<uint8_t>(payload_length >> 8));

  return header;
}

bool StdIOSocket::receive(const uint8_t* data, size_t length) {
  size_t consumed_data = 0;

  if (m_header.size() < m_header_length) {
    if (m_header.empty()) {
      consumed_data = min<size_t>(length, m_header_length);
      m_header.assign(data, data + consumed_data);
    } else {
      consumed_data = min<size_t>(length, m_header_length - m_header.size());
      m_header.insert(m_header.end(), data, data + consumed_data);
    }

    if (m_header.size() < m_header_length) {
      LOG.debug(to_string(m_payload.size()) + "/"  +  to_string(m_payload_length) + " bytes received (header)", "StdIOSocket");
      return false;
    }

    uint16_t magic_code = (m_header.at(0) << 8) | m_header.at(1);
    if (magic_code != MAGIC_CODE) {
      LOG.error("Packet with wrong magic code received: " + to_string(magic_code));
      m_header.clear();
      return false;
    }

    if (__BYTE_ORDER == __LITTLE_ENDIAN) {
      m_payload_length = (m_header.at(3) << 8) | m_header.at(2);
    } else {
      m_payload_length = (m_header.at(2) << 8) | m_header.at(3);
    }
  }

  m_payload.insert(m_payload.end(), data + consumed_data, data + min(consumed_data + m_payload_length, length));

  if (m_payload.size() < m_payload_length) {
    LOG.debug(to_string(m_payload.size()) + "/"  +  to_string(m_payload_length) + " bytes received", "StdIOSocket");
    return false;
  }

  return true;
}

void StdIOSocket::clear() {
  m_header.clear();
  m_payload_length = 0;
  m_payload.clear();
  m_payload.shrink_to_fit();
}
