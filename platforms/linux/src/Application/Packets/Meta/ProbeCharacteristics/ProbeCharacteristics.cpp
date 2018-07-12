#include "ProbeCharacteristics.hpp"
#include "Application/Packets/Commands/ReadByType/ReadByTypeResponse.hpp"
#include "Application/Packets/Errors/ErrorResponse/ErrorResponse.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Meta {

    ProbeCharacteristics::ProbeCharacteristics()
        : HostOnlyPacket(Packet::Id::ProbeCharacteristics, initial_packet_code()) {
      m_waiting_char_declaration = true;
      m_waiting_char_configuration = false;

      m_read_by_type_request_packet = make_shared<Packet::Commands::ReadByTypeRequest>(
          Format::HCI::GattUUID::CharacteristicDeclaration
      );
    }

    void ProbeCharacteristics::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::ProbeCharacteristics*>();

      m_connection_handle = payload->connection_handle();
      m_start_handle = payload->start_handle();
      m_end_handle = payload->end_handle();

      m_read_by_type_request_packet->set_controller_id(m_controller_id);
      m_read_by_type_request_packet->set_connection_handle(m_connection_handle);
      m_read_by_type_request_packet->set_handles(m_start_handle, m_end_handle);
    }

    vector<uint8_t> ProbeCharacteristics::serialize(FlatbuffersFormatBuilder& builder) const {
      vector<flatbuffers::Offset<BaBLE::Characteristic>> characteristics;
      characteristics.reserve(m_characteristics.size());

      for (auto& characteristic : m_characteristics) {
        auto uuid = builder.CreateString(Utils::format_uuid(characteristic.uuid));

        bool indicate = (characteristic.properties & 1 << 5) > 0;
        bool notify = (characteristic.properties & 1 << 4) > 0;
        bool write = (characteristic.properties & 1 << 3) > 0 | (characteristic.properties & 1 << 2) > 0;
        bool read = (characteristic.properties & 1 << 1) > 0;
        bool broadcast = (characteristic.properties & 1 << 0) > 0;

        bool notification_enabled = (characteristic.configuration & 1 << 0) > 0;
        bool indication_enabled = (characteristic.configuration & 1 << 1) > 0;

        auto characteristic_offset = BaBLE::CreateCharacteristic(
            builder,
            characteristic.handle,
            characteristic.value_handle,
            characteristic.config_handle,
            indicate,
            notify,
            write,
            read,
            broadcast,
            notification_enabled,
            indication_enabled,
            uuid
        );
        characteristics.push_back(characteristic_offset);
      }

      auto characteristics_vector = builder.CreateVector(characteristics);
      auto payload = BaBLE::CreateProbeCharacteristics(
          builder,
          m_connection_handle,
          m_start_handle,
          m_end_handle,
          characteristics_vector
      );

      return builder.build(payload, BaBLE::Payload::ProbeCharacteristics);
    }

    vector<uint8_t> ProbeCharacteristics::serialize(HCIFormatBuilder& builder) const {
      return m_read_by_type_request_packet->serialize(builder);
    }

    const string ProbeCharacteristics::stringify() const {
      stringstream result;

      result << "<ProbeCharacteristics> "
             << AbstractPacket::stringify() << ", ";

      for (auto it = m_characteristics.begin(); it != m_characteristics.end(); ++it) {
        result << "{ Handle: " << to_string(it->handle) << ", "
               << "Properties: " << to_string(it->properties) << ", "
               << "Value handle: " << to_string(it->value_handle) << ", "
               << "Config handle: " << to_string(it->config_handle) << ", "
               << "Configuration: " << to_string(it->configuration) << ", "
               << "UUID: " << Utils::format_uuid(it->uuid) << "} ";
        if (next(it) != m_characteristics.end()) {
          result << ", ";
        }
      }

      return result.str();
    }

    void ProbeCharacteristics::prepare(const shared_ptr<PacketRouter>& router) {
      // We want to keep the same Packet::Type to resend command until we probed all the characteristics
      if (m_waiting_char_declaration || m_waiting_char_configuration) {
        m_read_by_type_request_packet->translate();
        m_current_type = m_read_by_type_request_packet->get_type();

        PacketUuid response_uuid = m_read_by_type_request_packet->get_response_uuid();
        auto response_callback =
            [this](const shared_ptr<PacketRouter>& router, shared_ptr<Packet::AbstractPacket> packet) {
              return on_read_by_type_response_received(router, packet);
            };

        PacketUuid error_uuid = m_read_by_type_request_packet->get_response_uuid();
        error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
        auto error_callback =
            [this](const shared_ptr<PacketRouter>& router, shared_ptr<Packet::AbstractPacket> packet) {
              return on_error_response_received(router, packet);
            };

        router->add_callback(response_uuid, shared_from(this), response_callback);
        router->add_callback(error_uuid, shared_from(this), error_callback);
      } else {
        m_current_type = final_type();
      }
    }

    shared_ptr<AbstractPacket> ProbeCharacteristics::on_read_by_type_response_received(const shared_ptr<PacketRouter>& router,
                                                                                       const shared_ptr<AbstractPacket>& packet) {
      LOG.debug("Response received", "ProbeCharacteristics");
      PacketUuid error_uuid = m_read_by_type_request_packet->get_response_uuid();
      error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
      router->remove_callback(error_uuid);

      auto read_by_type_response_packet = dynamic_pointer_cast<Packet::Commands::ReadByTypeResponse>(packet);
      if (read_by_type_response_packet == nullptr) {
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::Failed,
            "Can't downcast AbstractPacket to ReadByTypeResponse packet (ProbeCharacteristics).",
            m_uuid_request
        );
      }

      vector<Format::HCI::Characteristic> new_characteristics = read_by_type_response_packet->get_characteristics();

      if (m_waiting_char_declaration) {
        m_characteristics.insert(m_characteristics.end(), new_characteristics.begin(), new_characteristics.end());

        uint16_t last_ending_handle = read_by_type_response_packet->get_last_ending_handle();
        if (last_ending_handle == m_end_handle) {
          m_waiting_char_declaration = false;
          m_waiting_char_configuration = true;
        } else {
          m_waiting_char_declaration = true;
          m_read_by_type_request_packet->set_handles(static_cast<uint16_t>(last_ending_handle + 1), m_end_handle);
        }
      } else {
        m_characteristics_config.insert(m_characteristics_config.end(), new_characteristics.begin(), new_characteristics.end());

        uint16_t last_ending_handle = read_by_type_response_packet->get_last_ending_handle();
        if (last_ending_handle == m_end_handle) {
          m_waiting_char_configuration = false;
          _merge_characteristics();
        } else {
          m_waiting_char_configuration = true;
          m_read_by_type_request_packet->set_handles(static_cast<uint16_t>(last_ending_handle + 1), m_end_handle);
        }
      }

      return shared_from(this);
    }

    shared_ptr<AbstractPacket> ProbeCharacteristics::on_error_response_received(const shared_ptr<PacketRouter>& router,
                                                                                const shared_ptr<AbstractPacket>& packet) {
      LOG.debug("Error received (could be normal)", "ProbeCharacteristics");
      PacketUuid response_uuid = m_read_by_type_request_packet->get_response_uuid();
      response_uuid.response_packet_code = Format::HCI::AttributeCode::ReadByTypeResponse;
      router->remove_callback(response_uuid);

      auto error_packet = dynamic_pointer_cast<Packet::Errors::ErrorResponse>(packet);
      if (error_packet == nullptr) {
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::Failed,
            "Can't downcast AbstractPacket to ErrorResponse packet (ProbeCharacteristics).",
            m_uuid_request
        );
      }

      Format::HCI::AttributeErrorCode error_code = error_packet->get_error_code();

      if (error_code != Format::HCI::AttributeErrorCode::AttributeNotFound) {
        import_status(error_packet);
        m_waiting_char_declaration = false;
        m_waiting_char_configuration = false;
        return shared_from(this);
      }

      if (m_waiting_char_declaration) {
        // 1st step done -> now we want the characteristics configurations
        m_waiting_char_declaration = false;
        m_waiting_char_configuration = true;
        m_read_by_type_request_packet->set_gatt_uuid(Format::HCI::GattUUID::ClientCharacteristicConfiguration);
        m_read_by_type_request_packet->set_handles(m_start_handle, m_end_handle);

      } else {
        // 2nd step done
        m_waiting_char_configuration = false;
        _merge_characteristics();
      }

      return shared_from(this);
    }

    void ProbeCharacteristics::_merge_characteristics() {
      auto it = m_characteristics.begin();

      for (auto config_char : m_characteristics_config) {
        while (config_char.config_handle > it->value_handle) {
          if(++it == m_characteristics.end()) break;
        }

        prev(it)->config_handle = config_char.config_handle;
        prev(it)->configuration = config_char.configuration;
      }

      m_characteristics_config.clear();
    }

  }

}