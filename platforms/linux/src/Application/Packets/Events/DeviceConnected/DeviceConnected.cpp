#include "DeviceConnected.hpp"
#include "Transport/Socket/HCI/HCISocket.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    DeviceConnected::DeviceConnected()
        : ControllerToHostPacket(Packet::Id::DeviceConnected, initial_type(), initial_packet_code(), final_packet_code()) {
      m_address_type = 0x00;
    }

    void DeviceConnected::unserialize(HCIFormatExtractor& extractor) {
      set_status(extractor.get_value<uint8_t>());
      m_connection_handle = extractor.get_value<uint16_t>();
      auto role = extractor.get_value<uint8_t>();
      m_address_type = static_cast<uint8_t>(extractor.get_value<uint8_t>() + 1);
      m_raw_address = extractor.get_array<uint8_t, 6>();

      m_address = Utils::format_bd_address(m_raw_address);
    }

    vector<uint8_t> DeviceConnected::serialize(FlatbuffersFormatBuilder& builder) const {
      auto address = builder.CreateString(m_address);

      auto payload = BaBLE::CreateDeviceConnected(
          builder,
          m_connection_handle,
          address,
          m_address_type
      );

      return builder.build(payload, BaBLE::Payload::DeviceConnected);
    }

    void DeviceConnected::set_socket(AbstractSocket* socket) {
      // This part is needed due to a bug since Linux Kernel v4 : we have to create manually the L2CAP socket, else
      // we'll be disconnected after sending one packet.
      if (m_status != BaBLE::StatusCode::Success) {
        LOG.info("Unsuccessful DeviceConnected event ignored (because the device is not connected...)", "DeviceConnected");
        return;
      }

      auto hci_socket = dynamic_cast<HCISocket*>(socket);
      if (hci_socket == nullptr) {
        throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Can't downcast socket to HCISocket packet");
      }

      try {
        hci_socket->connect_l2cap_socket(m_connection_handle, m_raw_address, m_address_type);
      } catch (const Exceptions::BaBLEException& err) {
        LOG.warning(err.get_message(), "DeviceConnected");
        set_status(Format::HCI::ConnectionFailedEstablished);
      }
    }

    const string DeviceConnected::stringify() const {
      stringstream result;

      result << "<DeviceConnected> "
             << AbstractPacket::stringify() << ", "
             << "Address: " << m_address << ", "
             << "Address type: " << to_string(m_address_type);

      return result.str();
    }

  }

}