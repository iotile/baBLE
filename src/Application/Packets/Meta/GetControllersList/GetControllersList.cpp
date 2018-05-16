#include "GetControllersList.hpp"

using namespace std;

namespace Packet::Meta {

  GetControllersList::GetControllersList(Packet::Type initial_type, Packet::Type translated_type)
      : AbstractPacket(initial_type, translated_type) {
    m_controller_info_packet = std::make_unique<Packet::Commands::GetControllerInfo>(translated_type, translated_type);
    m_controllers_ids_packet = std::make_unique<Packet::Commands::GetControllersIds>(translated_type, translated_type);

    m_waiting_response = SubPacket::GetControllersIds;
    m_current_index = 0;

    m_status = Schemas::StatusCode::Success;
    m_native_status = 0;
  }

  void GetControllersList::unserialize(AsciiFormatExtractor& extractor) {}
  void GetControllersList::unserialize(FlatbuffersFormatExtractor& extractor) {}

  vector<uint8_t> GetControllersList::serialize(AsciiFormatBuilder& builder) const {
    builder
        .set_name("GetControllersList")
        .add("Type", "Meta")
        .add("Num. of controllers", m_controllers.size());

    size_t i = 0;
    for (auto& controller : m_controllers) {
      builder
          .add("Controller ID", controller.id)
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

    return builder
        .set_status(m_status, m_native_status)
        .build(payload, Schemas::Payload::GetControllersList);
  }

  vector<uint8_t> GetControllersList::serialize(MGMTFormatBuilder& builder) const {
    switch (m_waiting_response) {
      case SubPacket::GetControllersIds:
        builder.set_controller_id(Format::MGMT::non_controller_id);
        return m_controllers_ids_packet->serialize(builder);

      case SubPacket::GetControllerInfo:
        builder.set_controller_id(m_controllers_ids.at(m_current_index));
        return m_controller_info_packet->serialize(builder);

      case SubPacket::None:
        throw std::runtime_error("Can't serialize 'GetControllersList' to MGMT.");
    }
  }

  void GetControllersList::translate() {
    switch (m_waiting_response) {
      case SubPacket::GetControllersIds:
      case SubPacket::GetControllerInfo:
        m_current_type = m_translated_type;
        break;

      case SubPacket::None:
        m_current_type = m_initial_type;
        break;
    }
  }

  uint64_t GetControllersList::expected_response_uuid() {
    switch (m_waiting_response) {
      case SubPacket::GetControllersIds:
        return Packet::AbstractPacket::compute_uuid(
            m_controllers_ids_packet->command_code(current_type()),
            Format::MGMT::non_controller_id
        );

      case SubPacket::GetControllerInfo:
        return Packet::AbstractPacket::compute_uuid(
            m_controller_info_packet->command_code(current_type()),
            m_controllers_ids.at(m_current_index)
        );

      case SubPacket::None:
        return 0;
    }
  }

  bool GetControllersList::on_response_received(Packet::Type packet_type, const std::vector<uint8_t>& raw_data) {
    if (packet_type != m_translated_type) {
      return false;
    }

    LOG.debug("Response received", "GetControllersList");

    if (m_waiting_response == SubPacket::GetControllersIds) {
      m_controllers_ids_packet->from_bytes(raw_data);

      std::tie(m_status, m_native_status, m_native_class) = m_controllers_ids_packet->get_full_status();

      if (m_status != Schemas::StatusCode::Success) {
        m_waiting_response = SubPacket::None;
        return true;
      }

      m_controllers_ids = m_controllers_ids_packet->get_controllers_ids();

      if (m_controllers_ids.empty()) {
        m_waiting_response = SubPacket::None;
        return true;
      }

      m_controllers.reserve(m_controllers_ids.size());
      m_current_index = 0;
      m_waiting_response = SubPacket::GetControllerInfo;

    } else if (m_waiting_response == SubPacket::GetControllerInfo) {
      m_controller_info_packet->from_bytes(raw_data);

      std::tie(m_status, m_native_status, m_native_class) = m_controller_info_packet->get_full_status();

      if (m_status != Schemas::StatusCode::Success) {
        m_waiting_response = SubPacket::None;
        return true;
      }

      m_controllers.push_back(m_controller_info_packet->get_controller_info());


      if (m_controllers.size() >= m_controllers_ids.size()) {
        m_waiting_response = SubPacket::None;
        return true;
      }

      m_current_index++;
    } else {
      return false;
    }

    return true;
  }

}