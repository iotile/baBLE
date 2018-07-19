#include "GetControllerInfoResponse.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetControllerInfoResponse::GetControllerInfoResponse()
        : ControllerToHostPacket(Packet::Id::GetControllerInfoResponse, initial_type(), initial_packet_code(), final_packet_code()) {}

    void GetControllerInfoResponse::unserialize(MGMTFormatExtractor& extractor) {
      set_status(extractor.get_value<uint8_t>());

      if (m_status == BaBLE::StatusCode::Success) {
        m_controller_info.id = m_controller_id;
        m_controller_info.address = extractor.get_array<uint8_t, 6>();
        m_controller_info.bluetooth_version = extractor.get_value<uint8_t>();
        m_controller_info.manufacturer = extractor.get_value<uint16_t>();
        m_controller_info.supported_settings = extractor.get_value<uint32_t>();
        m_controller_info.current_settings = extractor.get_value<uint32_t>();
        m_controller_info.class_of_device = extractor.get_array<uint8_t, 3>();

        array<uint8_t, 249> name_array = extractor.get_array<uint8_t, 249>();
        m_controller_info.name = Utils::bytes_to_string(name_array);

        array<uint8_t, 11> short_name_array = extractor.get_array<uint8_t, 11>();
        if (short_name_array[0] != 0) {
          m_controller_info.short_name = Utils::bytes_to_string(short_name_array);
        }
      }
    }

    vector<uint8_t> GetControllerInfoResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto address = builder.CreateString(Utils::format_bd_address(m_controller_info.address));
      auto name = builder.CreateString(m_controller_info.name);

      bool powered = (m_controller_info.current_settings & 1) > 0;
      bool connectable = (m_controller_info.current_settings & 1 << 1) > 0;
      bool discoverable = (m_controller_info.current_settings & 1 << 3) > 0;
      bool low_energy = (m_controller_info.current_settings & 9) > 0;
      // TODO: add advertising

      auto controller = BaBLE::CreateController(
          builder,
          m_controller_id,
          address,
          m_controller_info.bluetooth_version,
          powered,
          connectable,
          discoverable,
          low_energy,
          name
      );
      auto payload = BaBLE::CreateGetControllerInfo(builder, controller);

      return builder.build(payload, BaBLE::Payload::GetControllerInfo);
    }

    const string GetControllerInfoResponse::stringify() const {
      stringstream result;

      result << "<GetControllerInfoResponse> "
             << AbstractPacket::stringify() << ", "
             << "Controller ID: " << to_string(m_controller_info.id) << ", "
             << "Address: " << Utils::format_bd_address(m_controller_info.address) << ", "
             << "Bluetooth version: " << to_string(m_controller_info.bluetooth_version) << ", "
             << "Manufacturer: " << to_string(m_controller_info.manufacturer) << ", "
             << "Supported settings: " << to_string(m_controller_info.supported_settings) << ", "
             << "Current settings: " << to_string(m_controller_info.current_settings) << ", "
             << "Class of device: (" << m_controller_info.class_of_device[0] << ", "
                                     << m_controller_info.class_of_device[1] << ", "
                                     << m_controller_info.class_of_device[2] << "), "
             << "Name: " << m_controller_info.name << ", "
             << "Short name: " << m_controller_info.short_name;

      return result.str();
    }

  }

}