#include "GetControllersList.hpp"
#include "../../Commands/GetControllersIds/GetControllersIdsResponse.hpp"
#include "../../Commands/GetControllerInfo/GetControllerInfoResponse.hpp"
#include "../../../../Exceptions/RuntimeError/RuntimeErrorException.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Meta {

    GetControllersList::GetControllersList(Packet::Type initial_type, Packet::Type final_type)
        : AbstractPacket(initial_type, final_type) {
      m_id = Packet::Id::GetControllersList;
      m_packet_code = packet_code(m_current_type);

      m_controller_info_request_packet =
          std::make_shared<Packet::Commands::GetControllerInfoRequest>(final_type, final_type);
      m_controllers_ids_request_packet =
          std::make_shared<Packet::Commands::GetControllersIdsRequest>(final_type, final_type);

      m_waiting_response = SubPacket::GetControllersIds;
      m_current_index = 0;
    }

    void GetControllersList::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> GetControllersList::serialize(FlatbuffersFormatBuilder& builder) const {
      std::vector<flatbuffers::Offset<BaBLE::Controller>> controllers;
      controllers.reserve(m_controllers.size());

      for (auto& controller : m_controllers) {
        auto address = builder.CreateString(Utils::format_bd_address(controller.address));
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
          return m_controllers_ids_request_packet->serialize(builder);

        case SubPacket::GetControllerInfo:
          return m_controller_info_request_packet->serialize(builder);

        case SubPacket::None:
          throw std::runtime_error("Can't serialize 'GetControllersList' to MGMT.");
      }
    }

    const std::string GetControllersList::stringify() const {
      stringstream result;

      result << "<GetControllersList> "
             << AbstractPacket::stringify() << ", "
             << "Number of controllers: " << to_string(m_controllers.size()) << ", ";

      for (auto it = m_controllers.begin(); it != m_controllers.end(); ++it) {
        result << "{ Controller Id: " << to_string(it->id) << ", "
               << "Address: " << Utils::format_bd_address(it->address) << ", "
               << "Bluetooth version: " << to_string(it->bluetooth_version) << ", "
               << "Manufacturer: " << to_string(it->manufacturer) << ", "
               << "Supported settings: " << to_string(it->supported_settings) << ", "
               << "Current settings: " << to_string(it->current_settings) << ", "
               << "Class of device: (" << to_string(it->class_of_device[0]) << ", "
                                       << to_string(it->class_of_device[1]) << ", "
                                       << to_string(it->class_of_device[2]) << "), "
               << "Name: " << it->name << ", "
               << "Short name: " << it->short_name << "} ";
        if (next(it) != m_controllers.end()) {
          result << ", ";
        }
      }

      return result.str();
    }

    void GetControllersList::before_sent(const std::shared_ptr<PacketRouter>& router) {
      switch (m_waiting_response) {
        case GetControllersIds: {
          m_current_type = m_final_type;
          set_controller_id(NON_CONTROLLER_ID);
          m_controller_info_request_packet->set_controller_id(NON_CONTROLLER_ID);

          PacketUuid uuid = m_controllers_ids_request_packet->get_response_uuid();
          auto callback =
              [this](const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet) {
                return on_controllers_ids_response_received(router, packet);
              };
          router->add_callback(uuid, shared_from(this), callback);
          break;
        }

        case GetControllerInfo: {
          m_current_type = m_final_type;
          set_controller_id(m_controllers_ids.at(m_current_index));
          m_controller_info_request_packet->set_controller_id(m_controllers_ids.at(m_current_index));

          PacketUuid uuid = m_controller_info_request_packet->get_response_uuid();
          auto callback =
              [this](const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet) {
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

    shared_ptr<AbstractPacket> GetControllersList::on_controllers_ids_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                                        const shared_ptr<AbstractPacket>& packet) {
      LOG.debug("Controllers ids response received", "GetControllersList");

      auto controllers_ids_response_packet = dynamic_pointer_cast<Packet::Commands::GetControllersIdsResponse>(packet);
      if (controllers_ids_response_packet == nullptr) {
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

    shared_ptr<AbstractPacket> GetControllersList::on_controller_info_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                                        const std::shared_ptr<AbstractPacket>& packet) {
      LOG.debug("Controller info response received", "GetControllersList");

      auto controller_info_response_packet = dynamic_pointer_cast<Packet::Commands::GetControllerInfoResponse>(packet);
      if (controller_info_response_packet == nullptr) {
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

}