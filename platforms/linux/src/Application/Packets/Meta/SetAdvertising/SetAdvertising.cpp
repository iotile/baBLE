#include "SetAdvertising.hpp"
#include "Application/Packets/Events/CommandComplete/CommandComplete.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Meta {

    SetAdvertising::SetAdvertising()
        : HostOnlyPacket(Packet::Id::SetAdvertising, initial_packet_code()) {
      m_state = false;
      m_company_id = 0;

      m_waiting_response = Packet::Id::SetAdvertisingData;

      m_set_advertising_data_packet = make_shared<Commands::SetAdvertisingData>();
      m_set_scan_response_packet = make_shared<Commands::SetScanResponse>();
      m_set_advertising_params_packet = make_shared<Commands::SetAdvertisingParameters>();
      m_set_advertise_enable_packet = make_shared<Commands::SetAdvertiseEnable>(m_state);
    }

    void SetAdvertising::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::SetAdvertising*>();

      m_set_advertising_data_packet->set_controller_id(m_controller_id);
      m_set_scan_response_packet->set_controller_id(m_controller_id);
      m_set_advertising_params_packet->set_controller_id(m_controller_id);
      m_set_advertise_enable_packet->set_controller_id(m_controller_id);

      m_state = payload->state();

      if (m_state) {
        for (uint32_t i = 0; i < payload->uuids()->Length(); i++) {
          string uuid_str = payload->uuids()->Get(i)->str();
          vector<uint8_t> uuid = Utils::extract_uuid(uuid_str);

          if (uuid.size() == 2) {
            m_uuids_16_bits.push_back(uuid);
          } else if (uuid.size() == 16) {
            m_uuids_128_bits.push_back(uuid);
          } else {
            throw Exceptions::BaBLEException(
                BaBLE::StatusCode::WrongFormat,
                "UUID must be 16bit or 128bit, received_uuid=" + uuid_str,
                m_uuid_request
            );
          }
        }

        m_company_id = payload->company_id();
        m_name = payload->name()->str();
        m_adv_manufacturer_data.assign(payload->adv_manufacturer_data()->begin(), payload->adv_manufacturer_data()->end());
        m_scan_manufacturer_data.assign(payload->scan_manufacturer_data()->begin(), payload->scan_manufacturer_data()->end());

        m_set_advertising_data_packet->set_advertising_data(build_advertising_data());
        m_set_scan_response_packet->set_scan_response_data(build_scan_response());
      } else {
        m_waiting_response = Packet::Id::SetAdvertiseEnable;
      }

      m_set_advertise_enable_packet->set_state(m_state);
    }

    vector<uint8_t> SetAdvertising::build_advertising_data() {
      vector<uint8_t> advertising_data;
      advertising_data.reserve(
          3 +  // Flags
          m_uuids_16_bits.size() * 4 + m_uuids_128_bits.size() * 18 +  // UUIDs
          m_adv_manufacturer_data.size() + sizeof(m_company_id) + 2 // Manufacturer data
      );

      // Flags
      advertising_data.push_back(2);  // Length
      advertising_data.push_back(Format::HCI::ReportType::Flags);  // Type
      advertising_data.push_back(1 << 2 | 1 << 1);  // Value

      // UUIDs
      for (auto& uuid : m_uuids_16_bits) {
        advertising_data.push_back(3);  // Length
        advertising_data.push_back(Format::HCI::ReportType::UUID16ServiceClass);  // Type
        advertising_data.insert(advertising_data.end(), uuid.begin(), uuid.end());
      }

      for (auto& uuid : m_uuids_128_bits) {
        advertising_data.push_back(17);  // Length
        advertising_data.push_back(Format::HCI::ReportType::IncompleteUUID128ServiceClass);  // Type
        advertising_data.insert(advertising_data.end(), uuid.begin(), uuid.end());
      }

      // Manufacturer data
      if (!m_adv_manufacturer_data.empty()) {
        advertising_data.push_back(static_cast<uint8_t>(m_adv_manufacturer_data.size() + sizeof(m_company_id) + 1));  // Length
        advertising_data.push_back(Format::HCI::ReportType::ManufacturerSpecific);  // Type
        advertising_data.push_back(static_cast<uint8_t>((m_company_id & 0x00FF)));  // Company ID
        advertising_data.push_back(static_cast<uint8_t>(m_company_id >> 8));
        advertising_data.insert(advertising_data.end(), m_adv_manufacturer_data.begin(), m_adv_manufacturer_data.end());  // Data
      }

      return advertising_data;
    }

    vector<uint8_t> SetAdvertising::build_scan_response() {
      vector<uint8_t> scan_response_data;
      scan_response_data.reserve(
          m_scan_manufacturer_data.size() + sizeof(m_company_id) + 2 +  // Manufacturer data
          m_name.length() + 2  // Name
      );

      // Manufacturer data
      if (!m_scan_manufacturer_data.empty()) {
        scan_response_data.push_back(static_cast<uint8_t>(m_scan_manufacturer_data.size() + sizeof(m_company_id) + 1));  // Length
        scan_response_data.push_back(Format::HCI::ReportType::ManufacturerSpecific);  // Type
        scan_response_data.push_back(static_cast<uint8_t>((m_company_id & 0x00FF)));  // Company ID
        scan_response_data.push_back(static_cast<uint8_t>(m_company_id >> 8));
        scan_response_data.insert(scan_response_data.end(), m_scan_manufacturer_data.begin(), m_scan_manufacturer_data.end());  // Data
      }

      if (m_name.length() > 0) {
        vector<uint8_t> name_bytes = Utils::string_to_bytes(m_name);

        scan_response_data.push_back(static_cast<uint8_t>(name_bytes.size() + 1));  // Length
        scan_response_data.push_back(Format::HCI::ReportType::CompleteDeviceName);  // Type
        scan_response_data.insert(scan_response_data.end(), name_bytes.begin(), name_bytes.end());  // Company ID
      }

      return scan_response_data;
    }

    vector<uint8_t> SetAdvertising::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateSetAdvertising(builder, m_state);

      return builder.build(payload, BaBLE::Payload::SetAdvertising);
    }

    vector<uint8_t> SetAdvertising::serialize(HCIFormatBuilder& builder) const {
      switch(m_waiting_response) {
        case Packet::Id::SetAdvertisingData:
          return m_set_advertising_data_packet->serialize(builder);

        case Packet::Id::SetScanResponse:
          return m_set_scan_response_packet->serialize(builder);

        case Packet::Id::SetAdvertisingParameters:
          return m_set_advertising_params_packet->serialize(builder);

        case Packet::Id::SetAdvertiseEnable:
          return m_set_advertise_enable_packet->serialize(builder);

        default:
          throw Exceptions::BaBLEException(BaBLE::StatusCode::Unknown, "Unknown packet to serialize in SetAdvertising meta packet", m_uuid_request);
      }
    }

    const string SetAdvertising::stringify() const {
      stringstream result;

      result << "<SetAdvertising> "
             << AbstractPacket::stringify() << ", "
             << "State: " << m_state << ", "
             << "Company ID: " << to_string(m_company_id) << ", "
             << "Name: " << m_name << ", "
             << "Advertising manufacturer data: " << Utils::format_bytes_array(m_adv_manufacturer_data) << ", "
             << "Scan manufacturer data: " << Utils::format_bytes_array(m_scan_manufacturer_data) << ", "
             << "UUIDs 16 bits: ";

      for (auto it = m_uuids_16_bits.begin(); it != m_uuids_16_bits.end(); ++it) {
        result << Utils::format_uuid(*it);
        if (next(it) != m_uuids_16_bits.end()) {
          result << ", ";
        }
      }

      result << "UUIDs 128 bits: ";
      for (auto it = m_uuids_128_bits.begin(); it != m_uuids_128_bits.end(); ++it) {
        result << Utils::format_uuid(*it);
        if (next(it) != m_uuids_128_bits.end()) {
          result << ", ";
        }
      }

      return result.str();
    }

    Packet::Id SetAdvertising::next_step() {
      if (m_status != BaBLE::StatusCode::Success) {
        return Packet::Id::None;
      }

      switch(m_waiting_response) {
        case Packet::Id::SetAdvertisingData:
          return Packet::Id::SetScanResponse;

        case Packet::Id::SetScanResponse:
          return Packet::Id::SetAdvertisingParameters;

        case Packet::Id::SetAdvertisingParameters:
          return Packet::Id::SetAdvertiseEnable;

        case Packet::Id::SetAdvertiseEnable:
        default:
          return Packet::Id::None;
      }
    }

    void SetAdvertising::prepare(const shared_ptr<PacketRouter>& router) {
      switch(m_waiting_response) {

        case Packet::Id::SetAdvertisingData:
        {
          m_set_advertising_data_packet->translate();
          m_current_type = m_set_advertising_data_packet->get_type();

          PacketUuid response_uuid = m_set_advertising_data_packet->get_response_uuid();
          auto response_callback =
              [this](const shared_ptr<PacketRouter>& router, shared_ptr<AbstractPacket> packet) {
                return on_response_received(router, packet);
              };
          router->add_callback(response_uuid, shared_from(this), response_callback);
          break;
        }

        case Packet::Id::SetScanResponse:
        {
          m_set_scan_response_packet->translate();
          m_current_type = m_set_scan_response_packet->get_type();

          PacketUuid response_uuid = m_set_scan_response_packet->get_response_uuid();
          auto response_callback =
              [this](const shared_ptr<PacketRouter>& router, shared_ptr<AbstractPacket> packet) {
                return on_response_received(router, packet);
              };
          router->add_callback(response_uuid, shared_from(this), response_callback);
          break;
        }

        case Packet::Id::SetAdvertisingParameters:
        {
          m_set_advertising_params_packet->translate();
          m_current_type = m_set_advertising_params_packet->get_type();

          PacketUuid response_uuid = m_set_advertising_params_packet->get_response_uuid();
          auto response_callback =
              [this](const shared_ptr<PacketRouter>& router, shared_ptr<AbstractPacket> packet) {
                return on_response_received(router, packet);
              };

          router->add_callback(response_uuid, shared_from(this), response_callback);
          break;
        }

        case Packet::Id::SetAdvertiseEnable:
        {
          m_set_advertise_enable_packet->translate();
          m_current_type = m_set_advertise_enable_packet->get_type();

          PacketUuid response_uuid = m_set_advertise_enable_packet->get_response_uuid();
          auto response_callback =
              [this](const shared_ptr<PacketRouter>& router, shared_ptr<AbstractPacket> packet) {
                return on_response_received(router, packet);
              };

          router->add_callback(response_uuid, shared_from(this), response_callback);
          break;
        }

        default:
          m_current_type = final_type();
          break;

      }
    }

    shared_ptr<AbstractPacket> SetAdvertising::on_response_received(const shared_ptr<PacketRouter>& router,
                                                                    const shared_ptr<AbstractPacket>& packet) {
      LOG.debug("Advertising parameters response received", "SetAdvertising");
      auto command_complete_packet = dynamic_pointer_cast<Events::CommandComplete>(packet);
      if (command_complete_packet == nullptr) {
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::Failed,
            "Can't downcast AbstractPacket to CommandComplete packet (SetAvertising).",
            m_uuid_request
        );
      }

      vector<uint8_t> result = command_complete_packet->get_returned_params();
      if (result.empty()) {
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::WrongFormat,
            "Response received with no result (SetAvertising).",
            m_uuid_request
        );
      }

      set_status(result.at(0), true, "HCI");

      m_waiting_response = next_step();

      return shared_from(this);
    }

  }

}
