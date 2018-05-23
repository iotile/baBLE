#include "HCISocket.hpp"

using namespace std;
using namespace uvw;

Flags<PollHandle::Event> HCISocket::readable_flag = Flags<PollHandle::Event>(PollHandle::Event::READABLE);
Flags<PollHandle::Event> HCISocket::writable_flag = Flags<PollHandle::Event>(PollHandle::Event::WRITABLE);

HCISocket::HCISocket(shared_ptr<HCIFormat> format, uint16_t controller_id, const std::array<uint8_t, 6>& controller_address)
    : AbstractSocket(move(format)) {
  m_controller_id = controller_id;
  m_controller_address = controller_address;
  m_writable = true;

  m_hci_socket = socket(AF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC | SOCK_NONBLOCK, BTPROTO_HCI);
  if (m_hci_socket < 0) {
    throw Exceptions::SocketException("Error while creating the HCI socket: " + string(strerror(errno)));
  }

  if (!set_filter()) {
    throw Exceptions::SocketException("Error while setting filters on the HCI socket: " + string(strerror(errno)));
  }

  if(!bind_hci_socket()) {
    throw Exceptions::SocketException("Error while binding the HCI socket: " + string(strerror(errno)));
  }
}

bool HCISocket::bind_hci_socket() {
  struct Format::HCI::sockaddr_hci addr {
      AF_BLUETOOTH,
      m_controller_id,
      HCI_CHANNEL_RAW
  };

  return bind(m_hci_socket, (struct sockaddr*) &addr, sizeof(addr)) == 0;
}

bool HCISocket::set_filter() {
  struct Format::HCI::hci_filter filter {
    (1 << Format::HCI::Type::Event) | (1 << Format::HCI::Type::AsyncData),
    (1 << Format::HCI::EventCode::DisconnectComplete),
    (1 << Format::HCI::EventCode::LEMeta - 32),
    0
  };

  return setsockopt(m_hci_socket, SOL_HCI, HCI_FILTER, &filter, sizeof(filter)) == 0;
}

void HCISocket::connect_l2cap_socket(uint16_t connection_handle, const std::array<uint8_t, 6>& device_address, uint8_t device_address_type) {
  OSSocketHandle::Type l2cap_socket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
  if (l2cap_socket < 0) {
    throw Exceptions::SocketException("Error while creating the L2CAP socket: " + string(strerror(errno)));
  }

  struct Format::HCI::sockaddr_l2 addr{};
  addr.l2_family = AF_BLUETOOTH;
  copy(m_controller_address.begin(), m_controller_address.end(), std::begin(addr.l2_bdaddr.b));
  addr.l2_cid = ATT_CID;
  addr.l2_bdaddr_type = 0x01;

  if (bind(l2cap_socket, (struct sockaddr*)&addr, sizeof(addr)) != 0){
    throw Exceptions::SocketException("Error while binding L2CAP socket.");
  }

  addr.l2_family = AF_BLUETOOTH;
  copy(device_address.begin(), device_address.end(), std::begin(addr.l2_bdaddr.b));
  addr.l2_cid = ATT_CID;
  addr.l2_bdaddr_type = device_address_type;

  if (connect(l2cap_socket, (struct sockaddr *)&addr, sizeof(addr)) != 0){
    throw Exceptions::SocketException("Error while connecting L2CAP socket.");
  }

  LOG.info("L2CAP socket connected.", "HCISocket");
  m_l2cap_sockets.emplace(connection_handle, l2cap_socket);
}

void HCISocket::disconnect_l2cap_socket(uint16_t connection_handle) {
  auto it = m_l2cap_sockets.find(connection_handle);
  if (it != m_l2cap_sockets.end()) {
    OSSocketHandle::Type l2cap_socket = it->second;
    close(l2cap_socket);
    m_l2cap_sockets.erase(it);
    LOG.info("L2CAP socket manually disconnected.", "HCISocket");
  }
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
    if (write(m_hci_socket, data.data(), data.size()) < 0) {
      LOG.error("Error while sending a message to HCI socket: " + string(strerror(errno)), "HCISocket");
      throw Exceptions::SocketException("Error occured while sending the packet through HCI socket.");
    }
  }

  return true;
}

vector<uint8_t> HCISocket::receive() {
  uint8_t type_code = 0;
  ssize_t length_read;

  length_read = recv(m_hci_socket, &type_code, sizeof(type_code), MSG_PEEK);
  if (length_read != sizeof(type_code)) {
    LOG.error("Error while reading the type code: " + string(strerror(errno)), "HCISocket");
    throw Exceptions::SocketException("Can't read type code on the HCI socket.");
  }

  size_t header_length = m_format->get_header_length(type_code);

  vector<uint8_t> header(header_length);

  // MSG_PEEK is used to not remove data in socket queue. Else, for unknown reason, all the data are consumed and we
  // can't read the payload afterwards...
  length_read = recv(m_hci_socket, header.data(), header.size(), MSG_PEEK);

  if (length_read != header_length) {
    LOG.error("Error while reading the header: " + string(strerror(errno)), "HCISocket");
    throw Exceptions::SocketException("Can't read header on the HCI socket.");
  }

  size_t payload_length = m_format->extract_payload_length(header);

  vector<uint8_t> result(header_length + payload_length);
  length_read = read(m_hci_socket, result.data(), result.size());

  if (length_read != header_length + payload_length) {
    LOG.error("Error while reading the payload: " + string(strerror(errno)), "HCISocket");
    throw Exceptions::SocketException("Can't read payload on the HCI socket.");
  }

  return result;
}

void HCISocket::poll(shared_ptr<uvw::Loop> loop, OnReceivedCallback on_received, OnErrorCallback on_error) {
  m_poller = loop->resource<uvw::PollHandle>(m_hci_socket);

  m_poller->on<uvw::PollEvent>([this, on_received, on_error](const uvw::PollEvent& event, const uvw::PollHandle& handle){
    try {
      if (event.flags & uvw::PollHandle::Event::READABLE) {
        LOG.info("Reading data...", "HCISocket");
        vector<uint8_t> received_payload = receive();

        on_received(received_payload, m_format);

      } else if (event.flags & uvw::PollHandle::Event::WRITABLE) {
        set_writable(true);
        m_poller->start(HCISocket::readable_flag);
      }

    } catch (const Exceptions::AbstractException& err) {
      on_error(err);
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
  for (auto& kv : m_l2cap_sockets) {
    close(kv.second);
  }
  m_l2cap_sockets.clear();
  LOG.debug("L2CAP sockets closed", "HCI socket");

  close(m_hci_socket);
  LOG.debug("HCI socket closed", "HCI socket");
};
