#include "StdIOSocket.hpp"
#include "../../../Log/Log.hpp"
#include "../../../Exceptions/RuntimeError/RuntimeErrorException.hpp"

using namespace std;

StdIOSocket::StdIOSocket(shared_ptr<uvw::Loop>& loop, shared_ptr<AbstractFormat> format)
: AbstractSocket(move(format)) {
  m_header_length = 4;
  m_payload_length = 0;
  m_header.reserve(m_header_length);

  m_poller = loop->resource<uvw::PipeHandle>(false);
  m_poller->open(STDIO_ID::in);
}

bool StdIOSocket::send(const vector<uint8_t>& data) {
  vector<uint8_t> result = generate_header(data);
  result.insert(result.end(), data.begin(), data.end());

  fwrite(result.data(), sizeof(uint8_t), result.size(), stdout);
  fflush(stdout);
  LOG.debug("Data sent.", "StdIOSocket");
  return true;
}

void StdIOSocket::poll(OnReceivedCallback on_received, OnErrorCallback on_error) {
  m_poller->on<uvw::DataEvent>([this, on_received, on_error](const uvw::DataEvent& event, const uvw::PipeHandle& handle){
    LOG.debug("Readable data...", "StdIOSocket");
    size_t remaining_data_length = event.length;
    auto remaining_data = reinterpret_cast<uint8_t*>(event.data.get());

    while (remaining_data_length > 0) {
      try {
        LOG.debug("Remaining data: " + to_string(remaining_data_length), "StdIOSocket");
        if (!receive(remaining_data, remaining_data_length)) {
          return;
        }

        size_t consumed_data_length = m_header.size() + m_payload.size();
        if (consumed_data_length > remaining_data_length) {
          LOG.error("Wrong value of data length consumed (consumed data=" + to_string(consumed_data_length) +
                                                          ", remaining data=" + to_string(remaining_data_length));
          throw Exceptions::RuntimeErrorException("Error while receiving data on StdIO socket. Stream ignored");
        }

        remaining_data_length -= consumed_data_length;
        remaining_data += consumed_data_length;

        on_received(m_payload, m_format);
      } catch (const Exceptions::AbstractException& err) {
        on_error(err);
      }

      clear(); // To clean variables, before receiving other data in the next loop
    }
  });

  m_poller->read();
}

void StdIOSocket::on_close(OnCloseCallback on_close) {
  m_poller->on<uvw::EndEvent>([on_close](const uvw::EndEvent& event, const uvw::PipeHandle& handle){
    LOG.debug("StdIO socket closed...", "StdIOSocket");
    on_close();
  });

  m_poller->read();
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
