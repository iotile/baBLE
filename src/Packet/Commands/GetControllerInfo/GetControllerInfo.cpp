#include "GetControllerInfo.hpp"

using namespace std;

namespace Packet::Commands {

  GetControllerInfo::GetControllerInfo(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {
    m_bluetooth_version = 0;
    m_manufacturer = 0;
    m_supported_settings = 0;
    m_current_settings = 0;
  };

  void GetControllerInfo::import(AsciiFormatExtractor& extractor) {
    CommandPacket::import(extractor);

    try {
      m_controller_id = static_cast<uint16_t>(stoi(extractor.get()));

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Missing arguments for 'GetControllerInfo' packet. Usage: <command_code>,<controller_id>");
    } catch (const std::bad_cast& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'GetControllerInfo' packet. Can't cast.");
    } catch (const std::invalid_argument& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'GetControllerInfo' packet.");
    }
  };

  void GetControllerInfo::import(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::import(extractor);
    auto payload = extractor.get_payload<const Schemas::GetControllerInfo*>(Schemas::Payload::GetControllerInfo);

    m_controller_id = payload->controller_id();
  };

  void GetControllerInfo::import(MGMTFormatExtractor& extractor) {
    CommandPacket::import(extractor);

    if (m_native_status == 0) {
      m_address = extractor.get_array<uint8_t, 6>();
      m_bluetooth_version = extractor.get_value<uint8_t>();
      m_manufacturer = extractor.get_value<uint16_t>();
      m_supported_settings = extractor.get_value<uint32_t>();
      m_current_settings = extractor.get_value<uint32_t>();
      m_class_of_device = extractor.get_array<uint8_t, 3>();
      m_name = extractor.get_array<uint8_t, 249>();
      m_short_name = extractor.get_array<uint8_t, 11>();
    }
  };

  vector<uint8_t> GetControllerInfo::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
        .set_name("GetControllerInfo")
        .add("Address", AsciiFormat::format_bd_address(m_address))
        .add("Bluetooth version", m_bluetooth_version)
        .add("Manufacturer", m_manufacturer)
        .add("Supported settings", m_supported_settings)
        .add("Current settings", m_current_settings)
        .add("Class of device", m_class_of_device)
        .add("Name", AsciiFormat::bytes_to_string(m_name))
        .add("Short name", AsciiFormat::bytes_to_string(m_short_name));

    return builder.build();
  };

  vector<uint8_t> GetControllerInfo::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    auto address = builder.CreateString(AsciiFormat::format_bd_address(m_address));
    auto name = builder.CreateString(AsciiFormat::bytes_to_string(m_name));

    bool powered = (m_current_settings & 1) > 0;
    bool connectable = (m_current_settings & 1 << 1) > 0;
    bool discoverable = (m_current_settings & 1 << 3) > 0;
    bool low_energy = (m_current_settings & 9) > 0;

    auto payload = Schemas::CreateGetControllerInfo(
        builder,
        m_controller_id,
        address,
        m_bluetooth_version,
        powered,
        connectable,
        discoverable,
        low_energy,
        name
    );

    return builder.build(payload, Schemas::Payload::GetControllerInfo, m_native_class, m_status, m_native_status);
  }

  vector<uint8_t> GetControllerInfo::serialize(MGMTFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    return builder.build();
  };

}