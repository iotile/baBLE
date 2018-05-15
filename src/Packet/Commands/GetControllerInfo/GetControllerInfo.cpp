#include "GetControllerInfo.hpp"

using namespace std;

namespace Packet::Commands {

  GetControllerInfo::GetControllerInfo(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {
    m_controller_info = {};
  };

  void GetControllerInfo::unserialize(AsciiFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
  };

  void GetControllerInfo::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
  };

  void GetControllerInfo::unserialize(MGMTFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    if (m_native_status == 0) {
      m_controller_info.id = m_controller_id;

      std::array<uint8_t, 6> address = extractor.get_array<uint8_t, 6>();
      m_controller_info.address = AsciiFormat::format_bd_address(address);

      m_controller_info.bluetooth_version = extractor.get_value<uint8_t>();
      m_controller_info.manufacturer = extractor.get_value<uint16_t>();
      m_controller_info.supported_settings = extractor.get_value<uint32_t>();
      m_controller_info.current_settings = extractor.get_value<uint32_t>();
      m_controller_info.class_of_device = extractor.get_array<uint8_t, 3>();

      std::array<uint8_t, 249> name_array = extractor.get_array<uint8_t, 249>();
      m_controller_info.name = AsciiFormat::bytes_to_string(name_array);

      std::array<uint8_t, 11> short_name_array = extractor.get_array<uint8_t, 11>();
      if (short_name_array[0] != 0) {
        m_controller_info.short_name = AsciiFormat::bytes_to_string(short_name_array);
      }
    }
  };

  vector<uint8_t> GetControllerInfo::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
        .set_name("GetControllerInfo")
        .add("Controller ID", m_controller_info.id)
        .add("Address", m_controller_info.address)
        .add("Bluetooth version", m_controller_info.bluetooth_version)
        .add("Manufacturer", m_controller_info.manufacturer)
        .add("Supported settings", m_controller_info.supported_settings)
        .add("Current settings", m_controller_info.current_settings)
        .add("Class of device", m_controller_info.class_of_device)
        .add("Name", m_controller_info.name)
        .add("Short name", m_controller_info.short_name);

    return builder.build();
  };

  vector<uint8_t> GetControllerInfo::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    auto address = builder.CreateString(m_controller_info.address);
    auto name = builder.CreateString(m_controller_info.name);

    bool powered = (m_controller_info.current_settings & 1) > 0;
    bool connectable = (m_controller_info.current_settings & 1 << 1) > 0;
    bool discoverable = (m_controller_info.current_settings & 1 << 3) > 0;
    bool low_energy = (m_controller_info.current_settings & 9) > 0;

    auto controller = Schemas::CreateController(
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
    auto payload = Schemas::CreateGetControllerInfo(builder, controller);

    return builder.build(m_controller_id, payload, Schemas::Payload::GetControllerInfo, m_native_class, m_status, m_native_status);
  }

  vector<uint8_t> GetControllerInfo::serialize(MGMTFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    return builder.build();
  };

}