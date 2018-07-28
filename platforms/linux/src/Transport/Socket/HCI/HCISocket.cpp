#include "HCISocket.hpp"
#include "utils/string_formats.hpp"
#include "Log/Log.hpp"

using namespace std;

// Create one HCI socket per Bluetooth controller
vector<shared_ptr<HCISocket>> HCISocket::create_all(uv_loop_t* loop, shared_ptr<HCIFormat> hci_format) {
  vector<shared_ptr<HCISocket>> hci_sockets;

  Socket sock(AF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC, BTPROTO_HCI);

  struct Format::HCI::hci_dev_list_req* dl;
  struct Format::HCI::hci_dev_req* dr;

  dl = (Format::HCI::hci_dev_list_req*)malloc(HCI_MAX_DEV * sizeof(*dr) + sizeof(*dl));
  if (dl == nullptr) {
    throw runtime_error("Can't allocate memory to get HCI controllers list.");
  }

  try {
    dr = dl->dev_req;
    dl->dev_num = HCI_MAX_DEV;

    sock.ioctl(HCIGETDEVLIST, dl);

    for (int i = 0; i < dl->dev_num; i++, dr++) {
      shared_ptr<HCISocket> hci_socket = make_shared<HCISocket>(loop, hci_format, dr->dev_id);
      hci_sockets.push_back(hci_socket);
    }

    free(dl);

  } catch (const exception& err) {
    free(dl);
    throw;
  }

  return hci_sockets;
}

HCISocket::HCISocket(uv_loop_t* loop, shared_ptr<HCIFormat> format, uint16_t controller_id)
    : HCISocket(loop, move(format), controller_id, make_shared<Socket>(AF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC | SOCK_NONBLOCK, BTPROTO_HCI))
{}

HCISocket::HCISocket(uv_loop_t* loop, shared_ptr<HCIFormat> format, uint16_t controller_id, shared_ptr<Socket> hci_socket)
    : AbstractSocket(move(format)),
      m_hci_socket(move(hci_socket)) {
  m_controller_id = controller_id;
  m_writable = true;

  LOG.debug("Setting HCI filters...", "HCISocket");
  set_filters();

  LOG.debug("Binding HCI socket to controller #" + to_string(m_controller_id) + "...", "HCISocket");
  m_hci_socket->bind(m_controller_id, HCI_CHANNEL_RAW);

  LOG.debug("Getting HCI controller address...", "HCISocket");
  get_controller_info();

  LOG.debug("Setting up poller on HCI socket...", "HCISocket");
  m_poller = make_unique<uv_poll_t>();
  m_poller->data = this;
  uv_poll_init_socket(loop, m_poller.get(), static_cast<uv_os_sock_t>(m_hci_socket->get_raw()));

  LOG.debug("HCI socket created on " + Utils::format_bd_address(m_controller_address), "HCISocket");
}

bool HCISocket::get_controller_info() {
  struct Format::HCI::hci_dev_info di{};
  di.dev_id = m_controller_id;

  // To get the controller address
  m_hci_socket->ioctl(HCIGETDEVINFO, (void *)&di);
  m_buffer_size = di.acl_pkts;
  copy(begin(di.bdaddr.b), end(di.bdaddr.b), m_controller_address.begin());
  return true;
}

bool HCISocket::set_filters() {
  // IMPORTANT: without these filters, nothing will be received on the HCI socket...
  struct Format::HCI::hci_filter filter {
      (1 << Format::HCI::Type::Event) | (1 << Format::HCI::Type::AsyncData),
      (1 << Format::HCI::EventCode::DisconnectComplete) | (1 << Format::HCI::EventCode::CommandComplete) | (1 << Format::HCI::EventCode::CommandStatus) | ( 1 << Format::HCI::EventCode::NumberOfCompletedPackets),
      (1 << Format::HCI::EventCode::LEMeta - 32),
      0
  };

  m_hci_socket->set_option(SOL_HCI, HCI_FILTER, &filter, sizeof(filter));
}

void HCISocket::connect_l2cap_socket(uint16_t connection_handle, const array<uint8_t, 6>& device_address, uint8_t device_address_type) {
  Socket l2cap_socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
  l2cap_socket.bind(m_controller_address, 0x01, ATT_CID);
  l2cap_socket.connect(device_address, device_address_type, ATT_CID);

  LOG.info("L2CAP socket connected.", "HCISocket");
  m_l2cap_sockets.emplace(connection_handle, l2cap_socket);
}

void HCISocket::disconnect_l2cap_socket(uint16_t connection_handle) {
  auto it = m_l2cap_sockets.find(connection_handle);
  if (it != m_l2cap_sockets.end()) {
    Socket& l2cap_socket = it->second;
    l2cap_socket.close();
    m_l2cap_sockets.erase(it);
    LOG.info("L2CAP socket manually disconnected.", "HCISocket");
  }

  m_in_progress_packets.erase(connection_handle);
}

void HCISocket::set_gatt_table(const vector<Format::HCI::Service>& services, const vector<Format::HCI::Characteristic>& characteristics) {
  m_services = services;
  m_characteristics = characteristics;
}

vector<Format::HCI::Service> HCISocket::get_services() const {
  return m_services;
}

vector<Format::HCI::Characteristic> HCISocket::get_characteristics() const {
  return m_characteristics;
}

string HCISocket::get_controller_address() {
  return Utils::format_bd_address(m_controller_address);
}

bool HCISocket::send(const vector<uint8_t>& data, uint16_t connection_handle) {
  if (!m_writable) {
    LOG.debug("Already sending a message. Queuing...", "HCISocket");
    m_send_queue.push(make_tuple(data, connection_handle));
    return false;

  } else {
    if (connection_handle != 0) {
      uint16_t num_in_progress_packets = 0;
      for (auto& i : m_in_progress_packets) {
        num_in_progress_packets += i.second;
      }

      if (num_in_progress_packets >= m_buffer_size) {
        throw Exceptions::BaBLEException(BaBLE::StatusCode::Rejected, "Controller buffer full");
      }

//      LOG.debug(to_string(m_buffer_size - num_in_progress_packets) + " remaining slots in controller buffer.", "HCISocket");
    }

//    LOG.debug("Sending data...", "HCISocket");
//    LOG.debug(data, "HCISocket");
    set_writable(false);
    m_hci_socket->write(data);

    if (connection_handle != 0) {
      m_in_progress_packets[connection_handle]++;
    }
  }

  return true;
}

vector<uint8_t> HCISocket::receive() {
  // MSG_PEEK is used to not remove data in socket queue. Else, for unknown reason, all the data are consumed and we
  // can't read the payload afterwards...
  vector<uint8_t> type_code(1);
  m_hci_socket->read(type_code, true);

  size_t header_length = m_format->get_header_length(type_code.at(0));

  vector<uint8_t> header(header_length);
  m_hci_socket->read(header, true);

  size_t payload_length = m_format->extract_payload_length(header);

  vector<uint8_t> result(header_length + payload_length);
  m_hci_socket->read(result, false);

  return result;
}

void HCISocket::on_poll(uv_poll_t* handle, int status, int events) {
  if (status < 0) {
    LOG.error("Error while polling HCI socket: " + string(uv_err_name(status)), "HCISocket");
    return;
  }

  auto hci_socket = static_cast<HCISocket*>(handle->data);

  try {
    if (events & UV_READABLE) {
      vector<uint8_t> received_payload = hci_socket->receive();
//      LOG.debug("Data received", "HCISocket");
//      LOG.debug(received_payload, "HCISocket");
      hci_socket->m_on_received(received_payload, hci_socket);

    } else if (events & UV_WRITABLE) {
      hci_socket->set_writable(true);
    }

  } catch (const Exceptions::BaBLEException& err) {
    hci_socket->m_on_error(err);
  }
}

void HCISocket::poll(OnReceivedCallback on_received, OnErrorCallback on_error) {
  m_on_received = move(on_received);
  m_on_error = move(on_error);

  uv_poll_start(m_poller.get(), UV_READABLE | UV_WRITABLE, on_poll);
  m_poll_started = true;
}

void HCISocket::set_writable(bool is_writable) {
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
      tuple<vector<uint8_t>, uint16_t> to_send = m_send_queue.front();
      send(get<0>(to_send), get<1>(to_send));
      m_send_queue.pop();
    }
  }
}

void HCISocket::set_in_progress_packets(uint16_t connection_handle, uint16_t num_packets_processed) {
  auto it= m_in_progress_packets.find(connection_handle);
  if (it == m_in_progress_packets.end()) {
    LOG.warning("Connection handle not found : can't set the number of processing packets", "HCISocket");
    return;
  }

  if (num_packets_processed > it->second) {
    it->second = 0;
  } else {
    it->second -= num_packets_processed;
  }

//  LOG.debug(to_string(num_packets_processed) + " packets processed.", "HCISocket");
}

void HCISocket::close() {
  for (auto& kv : m_l2cap_sockets) {
    kv.second.close();
  }
  m_l2cap_sockets.clear();
  LOG.debug("L2CAP sockets closed", "HCISocket");

  m_hci_socket->close();
  LOG.debug("HCI socket closed", "HCISocket");
}

HCISocket::~HCISocket() {
  close();
}
