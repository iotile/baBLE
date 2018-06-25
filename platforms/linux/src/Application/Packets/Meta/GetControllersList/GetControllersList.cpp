#include "GetControllersList.hpp"
#include "../../Commands/GetControllersIds/GetControllersIdsResponse.hpp"
#include "../../Commands/GetControllerInfo/GetControllerInfoResponse.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Meta {

    GetControllersList::GetControllersList()
        : HostOnlyPacket(Packet::Id::GetControllersList, initial_packet_code()) {
      m_controller_info_request_packet = make_shared<Packet::Commands::GetControllerInfoRequest>();
      m_controllers_ids_request_packet = make_shared<Packet::Commands::GetControllersIdsRequest>();

      m_waiting_response = Packet::Id::GetControllersIdsResponse;
      m_current_index = 0;
    }

    void GetControllersList::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> GetControllersList::serialize(FlatbuffersFormatBuilder& builder) const {
      vector<flatbuffers::Offset<BaBLE::Controller>> controllers;
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
        case Packet::Id::GetControllersIdsResponse:
          return m_controllers_ids_request_packet->serialize(builder);

        case Packet::Id::GetControllerInfoResponse:
          return m_controller_info_request_packet->serialize(builder);

        default:
          throw runtime_error("Can't serialize 'GetControllersList' to MGMT.");
      }
    }

    const string GetControllersList::stringify() const {
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

    void GetControllersList::prepare(const shared_ptr<PacketRouter>& router) {
      switch (m_waiting_response) {
        case Packet::Id::GetControllersIdsResponse: {
          m_controllers_ids_request_packet->translate();
          m_current_type = m_controllers_ids_request_packet->get_type();
          set_controller_id(NON_CONTROLLER_ID);
          m_controllers_ids_request_packet->set_controller_id(NON_CONTROLLER_ID);

          PacketUuid uuid = m_controllers_ids_request_packet->get_response_uuid();
          auto callback =
              [this](const shared_ptr<PacketRouter>& router, shared_ptr<Packet::AbstractPacket> packet) {
                return on_controllers_ids_response_received(router, packet);
              };
          router->add_callback(uuid, shared_from(this), callback);
          break;
        }

        case Packet::Id::GetControllerInfoResponse: {
          m_controller_info_request_packet->translate();
          m_current_type = m_controller_info_request_packet->get_type();
          set_controller_id(m_controllers_ids.at(m_current_index));
          m_controller_info_request_packet->set_controller_id(m_controllers_ids.at(m_current_index));

          PacketUuid uuid = m_controller_info_request_packet->get_response_uuid();
          auto callback =
              [this](const shared_ptr<PacketRouter>& router, shared_ptr<Packet::AbstractPacket> packet) {
                return on_controller_info_response_received(router, packet);
              };
          router->add_callback(uuid, shared_from(this), callback);
          break;
        }

        default:
          m_current_type = final_type();
          break;
      }
    }

    shared_ptr<AbstractPacket> GetControllersList::on_controllers_ids_response_received(const shared_ptr<PacketRouter>& router,
                                                                                        const shared_ptr<AbstractPacket>& packet) {
      LOG.debug("Controllers ids response received", "GetControllersList");

      auto controllers_ids_response_packet = dynamic_pointer_cast<Packet::Commands::GetControllersIdsResponse>(packet);
      if (controllers_ids_response_packet == nullptr) {
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::Failed,
            "Can't downcast AbstractPacket to GetControllersIds packet (GetControllersList).",
            m_uuid_request
        );
      }

      import_status(controllers_ids_response_packet);

      if (m_status != BaBLE::StatusCode::Success) {
        m_waiting_response = Packet::Id::None;
      } else {
        m_controllers_ids = controllers_ids_response_packet->get_controllers_ids();

        if (m_controllers_ids.empty()) {
          m_waiting_response = Packet::Id::None;
        } else {
          m_controllers.reserve(m_controllers_ids.size());
          m_current_index = 0;
          // Now we want to get the first controller information
          m_waiting_response = Packet::Id::GetControllerInfoResponse;
        }
      }

      return shared_from(this);
    }

    shared_ptr<AbstractPacket> GetControllersList::on_controller_info_response_received(const shared_ptr<PacketRouter>& router,
                                                                                        const shared_ptr<AbstractPacket>& packet) {
      LOG.debug("Controller info response received", "GetControllersList");

      auto controller_info_response_packet = dynamic_pointer_cast<Packet::Commands::GetControllerInfoResponse>(packet);
      if (controller_info_response_packet == nullptr) {
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::Failed,
            "Can't downcast AbstractPacket to GetControllerInfo packet (GetControllersList).",
            m_uuid_request
        );
      }

      import_status(controller_info_response_packet);

      if (m_status != BaBLE::StatusCode::Success) {
        m_waiting_response = Packet::Id::None;
      } else {
        m_controllers.push_back(controller_info_response_packet->get_controller_info());

        if (m_controllers.size() >= m_controllers_ids.size()) {
          // We get all the controllers information: we have finished
          m_waiting_response = Packet::Id::None;
        } else {
          m_waiting_response = Packet::Id::GetControllerInfoResponse;
          m_current_index++;
        }
      }

      return shared_from(this);
    }

  }

}