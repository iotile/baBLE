#include <sstream>
#include "GetConnectedDevices.hpp"
#include "Exceptions/BaBLEException.hpp"
#include "utils/string_formats.hpp"
#include "Format/HCI/constants.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetConnectedDevices::GetConnectedDevices()
        : HostOnlyPacket(Packet::Id::GetConnectedDevices, initial_packet_code()) {}

    void GetConnectedDevices::unserialize(FlatbuffersFormatExtractor& extractor) {
      struct Format::HCI::hci_conn_list_req* cl;
      struct Format::HCI::hci_conn_info* ci;

      Socket sock(AF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC, BTPROTO_HCI);

      cl = (Format::HCI::hci_conn_list_req*)malloc(HCI_MAX_CONN * sizeof(*ci) + sizeof(*cl));
      if (cl == nullptr) {
        throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Can't allocate memory to get HCI controllers list.", m_uuid_request);
      }

      try {
        cl->dev_id = m_controller_id;
        cl->conn_num = HCI_MAX_CONN;
        ci = cl->conn_info;

        sock.ioctl(HCIGETCONNLIST, cl);

        for (int i = 0; i < cl->conn_num; i++, ci++) {
          Format::HCI::Device device{};
          device.connection_handle = ci->handle;
          copy(begin(ci->bdaddr.b), end(ci->bdaddr.b), device.address.begin());

          m_connected_devices.push_back(device);
        }

        free(cl);

      } catch (const exception& err) {
        free(cl);
        throw;
      }
    }

    vector<uint8_t> GetConnectedDevices::serialize(FlatbuffersFormatBuilder& builder) const {
      vector<flatbuffers::Offset<BaBLE::Device>> devices;
      devices.reserve(m_connected_devices.size());

      for (auto& connected_device : m_connected_devices) {
        auto address = builder.CreateString(Utils::format_bd_address(connected_device.address));

        auto device_offset = BaBLE::CreateDevice(
            builder,
            connected_device.connection_handle,
            address
        );
        devices.push_back(device_offset);
      }

      auto devices_vector = builder.CreateVector(devices);
      auto payload = BaBLE::CreateGetConnectedDevices(builder, devices_vector);

      return builder.build(payload, BaBLE::Payload::GetConnectedDevices);
    }

    const string GetConnectedDevices::stringify() const {
      stringstream result;

      result << "<GetConnectedDevices> "
             << AbstractPacket::stringify()
             << "Number of connections: " << to_string(m_connected_devices.size()) << ", ";

      for (auto it = m_connected_devices.begin(); it != m_connected_devices.end(); ++it) {
        result << "{ Connection handle: " << to_string(it->connection_handle) << ", "
               << "Address: " << Utils::format_bd_address(it->address) << "} ";
        if (next(it) != m_connected_devices.end()) {
          result << ", ";
        }
      }

      return result.str();
    }

  }

}