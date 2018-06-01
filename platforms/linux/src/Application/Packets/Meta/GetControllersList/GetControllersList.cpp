#include "GetControllersList.hpp"

using namespace std;

namespace Packet::Meta {

  GetControllersList::GetControllersList(Packet::Type initial_type, Packet::Type translated_type)
      : AbstractPacket(initial_type, translated_type) {
    m_id = Packet::Id::GetControllersList;
    m_packet_code = packet_code(m_current_type);

    m_controller_info_request_packet = std::make_shared<Packet::Commands::GetControllerInfoRequest>(translated_type, translated_type);
    m_controllers_ids_request_packet = std::make_shared<Packet::Commands::GetControllersIdsRequest>(translated_type, translated_type);

    m_waiting_response = SubPacket::GetControllersIds;
    m_current_index = 0;
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
          .add("Address", AsciiFormat::format_bd_address(controller.address))
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
    std::vector<flatbuffers::Offset<BaBLE::Controller>> controllers;
    controllers.reserve(m_controllers.size());

    for (auto& controller : m_controllers) {
      auto address = builder.CreateString(AsciiFormat::format_bd_address(controller.address));
      auto name = builder.CreateString(controller.name);

      bool powered = (controller.current_settings & 1) > 0;
      bool connectable = (controller.current_settings & 1 << 1) > 0;
      bool discoverable = (controller.current_settings & 1 << 3) > 0;
      bool low_energy = (controller.current_settings & 9) > 0;

      auto controller_offset = BaBLE::CreateController(
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
    auto payload = BaBLE::CreateGetControllersList(builder, controllers_vector);

    return builder.build(payload, BaBLE::Payload::GetControllersList);
  }

  vector<uint8_t> GetControllersList::serialize(MGMTFormatBuilder& builder) const {
    switch (m_waiting_response) {
      case SubPacket::GetControllersIds:
        builder.set_controller_id(NON_CONTROLLER_ID);
        return m_controllers_ids_request_packet->serialize(builder);

      case SubPacket::GetControllerInfo:
        builder.set_controller_id(m_controllers_ids.at(m_current_index));
        return m_controller_info_request_packet->serialize(builder);

      case SubPacket::None:
        throw std::runtime_error("Can't serialize 'GetControllersList' to MGMT.");
    }
  }

  void GetControllersList::before_sent(const std::shared_ptr<PacketRouter>& router) {
    switch (m_waiting_response) {
      case GetControllersIds:
      {
        m_current_type = m_translated_type;

        PacketUuid uuid = m_controllers_ids_request_packet->get_uuid();
        auto callback = [this](const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet) {
          return on_controllers_ids_response_received(router, packet);
        };
        router->add_callback(uuid, shared_from(this), callback);
        break;
      }

      case GetControllerInfo:
      {
        m_current_type = m_translated_type;

        PacketUuid uuid = m_controller_info_request_packet->get_uuid();
        uuid.controller_id = m_controllers_ids.at(m_current_index);
        auto callback = [this](const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet) {
          return on_controller_info_response_received(router, packet);
        };
        router->add_callback(uuid, shared_from(this), callback);
        break;
      }

      case None:
        m_current_type = m_initial_type;
        break;
    }
  }

  shared_ptr<AbstractPacket> GetControllersList::on_controllers_ids_response_received(const std::shared_ptr<PacketRouter>& router, const shared_ptr<AbstractPacket>& packet) {
    LOG.debug("Controllers ids response received", "GetControllersList");

    auto controllers_ids_response_packet = dynamic_pointer_cast<Packet::Commands::GetControllersIdsResponse>(packet);
    if(controllers_ids_response_packet == nullptr) {
      throw Exceptions::RuntimeErrorException("Can't downcast AbstractPacket to GetControllersIds packet.");
    }

    import_status(*controllers_ids_response_packet);

    if (m_status != BaBLE::StatusCode::Success) {
      m_waiting_response = SubPacket::None;
    } else {
      m_controllers_ids = controllers_ids_response_packet->get_controllers_ids();

      if (m_controllers_ids.empty()) {
        m_waiting_response = SubPacket::None;
      } else {
        m_controllers.reserve(m_controllers_ids.size());
        m_current_index = 0;
        // Now we want to get the first controller information
        m_waiting_response = SubPacket::GetControllerInfo;
      }
    }

    return shared_from(this);
  }

  shared_ptr<AbstractPacket> GetControllersList::on_controller_info_response_received(const std::shared_ptr<PacketRouter>& router, const std::shared_ptr<AbstractPacket>& packet) {
    LOG.debug("Controller info response received", "GetControllersList");

    auto controller_info_response_packet = dynamic_pointer_cast<Packet::Commands::GetControllerInfoResponse>(packet);
    if(controller_info_response_packet == nullptr) {
      throw Exceptions::RuntimeErrorException("Can't downcast AbstractPacket to GetControllerInfo packet.");
    }

    import_status(*controller_info_response_packet);

    if (m_status != BaBLE::StatusCode::Success) {
      m_waiting_response = SubPacket::None;
    } else {
      m_controllers.push_back(controller_info_response_packet->get_controller_info());

      if (m_controllers.size() >= m_controllers_ids.size()) {
        // We get all the controllers information: we have finished
        m_waiting_response = SubPacket::None;
      } else {
        m_waiting_response = SubPacket::GetControllerInfo;
        m_current_index++;
      }
    }

    return shared_from(this);
  }

}