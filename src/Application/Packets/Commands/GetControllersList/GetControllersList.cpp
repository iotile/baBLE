#include "GetControllersList.hpp"

using namespace std;

namespace Packet::Commands {

  GetControllersList::GetControllersList(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {
    m_waiting_controllers_info = false;
    m_current_index = 0;
  }

  void GetControllersList::unserialize(AsciiFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
  }

  void GetControllersList::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
  }

  void GetControllersList::unserialize(MGMTFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    if (m_native_status == 0) {
      auto m_num_controllers = extractor.get_value<uint16_t>();
      m_controllers_ids = extractor.get_vector<uint16_t>(m_num_controllers);
      m_controllers.reserve(m_num_controllers);
    }
  }

  vector<uint8_t> GetControllersList::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
        .set_name("GetControllersList")
        .add("Num. of controllers", m_controllers_ids.size())
        .add("Controllers ID", m_controllers_ids);

    size_t i = 0;
    for (auto& controller : m_controllers) {
      builder
          .add("Controller ID", m_controllers_ids.at(i))
          .add("Address", controller.address)
          .add("Bluetooth version", controller.bluetooth_version)
          .add("Manufacturer", controller.manufacturer)
          .add("Supported settings", controller.supported_settings)
          .add("Current settings", controller.current_settings)
          .add("Class of device", controller.class_of_device)
          .add("Name", controller.name)
          .add("Short name", controller.short_name);
      i++;
    }

    return builder.build();
  }

  vector<uint8_t> GetControllersList::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    std::vector<flatbuffers::Offset<Schemas::Controller>> controllers;
    controllers.reserve(m_controllers.size());

    for (auto& controller : m_controllers) {
      auto address = builder.CreateString(controller.address);
      auto name = builder.CreateString(controller.name);

      bool powered = (controller.current_settings & 1) > 0;
      bool connectable = (controller.current_settings & 1 << 1) > 0;
      bool discoverable = (controller.current_settings & 1 << 3) > 0;
      bool low_energy = (controller.current_settings & 9) > 0;

      auto controller_offset = Schemas::CreateController(
          builder,
          controller.id,
          address,
          controller.bluetooth_version,
          powered,
          connectable,
          discoverable,
          low_energy,
          name
      );
      controllers.push_back(controller_offset);
    }

    auto controllers_vector = builder.CreateVector(controllers);
    auto payload = Schemas::CreateGetControllersList(builder, controllers_vector);

    return builder.build(payload, Schemas::Payload::GetControllersList);
  }

  vector<uint8_t> GetControllersList::serialize(MGMTFormatBuilder& builder) const {
    if (m_waiting_controllers_info) {
      builder.set_controller_id(m_controllers_ids.at(m_current_index));
      return m_controller_info_packet->serialize(builder);

    } else {
      CommandPacket::serialize(builder);
      return builder.build();
    }
  }

  void GetControllersList::translate() {
    if (!m_waiting_controllers_info) {
      AbstractPacket::translate();
    }
  }

  uint64_t GetControllersList::expected_response_uuid() {
    if (!m_waiting_controllers_info) {
      return Packet::AbstractPacket::compute_uuid(command_code(m_translated_type), m_controller_id);

    } else if (m_controllers.size() < m_controllers_ids.size()){
      return Packet::AbstractPacket::compute_uuid(
          GetControllerInfo::command_code(m_translated_type),
          m_controllers_ids.at(m_current_index)
      );

    } else {
      return 0;
    }
  }

  bool GetControllersList::on_response_received(Packet::Type packet_type, const std::vector<uint8_t>& raw_data) {
    if (!m_waiting_controllers_info) {
      import(raw_data);

      if (!m_controllers_ids.empty()) {
        m_controller_info_packet = std::make_unique<GetControllerInfo>(packet_type, packet_type);
        m_waiting_controllers_info = true;
      }

      return true;
    }

    m_controller_info_packet->import(raw_data);

    std::tie(m_status, m_native_status, m_native_class) = m_controller_info_packet->get_full_status();

    if (m_status != Schemas::StatusCode::Success) {
      m_waiting_controllers_info = false;
      m_controller_info_packet.reset();
      return true;
    }

    m_controllers.push_back(m_controller_info_packet->get_controller_info());

    if (m_controllers.size() >= m_controllers_ids.size()) {
      m_waiting_controllers_info = false;
      m_controller_info_packet.reset();
      return true;
    }

    m_current_index++;

    return true;
  }

}