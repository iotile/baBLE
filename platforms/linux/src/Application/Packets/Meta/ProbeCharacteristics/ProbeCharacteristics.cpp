#include "ProbeCharacteristics.hpp"

using namespace std;

namespace Packet::Meta {

  ProbeCharacteristics::ProbeCharacteristics(Packet::Type initial_type, Packet::Type translated_type)
      : AbstractPacket(initial_type, translated_type) {
    m_id = Packet::Id::ProbeCharacteristics;
    m_packet_code = packet_code(m_current_type);

    m_waiting_characteristics = true;
    m_read_by_type_request_packet = make_unique<Packet::Commands::ReadByTypeRequest>(translated_type, translated_type);
  }

  void ProbeCharacteristics::unserialize(AsciiFormatExtractor& extractor) {
    try {
      m_connection_id = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'ProbeCharacteristics' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,<connection_handle>", m_uuid_request);
    }
  }

  void ProbeCharacteristics::unserialize(FlatbuffersFormatExtractor& extractor) {
    auto payload = extractor.get_payload<const BaBLE::ProbeCharacteristics*>();

    m_connection_id = payload->connection_handle();
  }

  vector<uint8_t> ProbeCharacteristics::serialize(AsciiFormatBuilder& builder) const {
    builder
        .set_name("ProbeCharacteristics")
        .add("Type", "Meta")
        .add("Connection id", m_connection_id);

    for (auto& characteristic : m_characteristics) {
      builder
          .add("Handle", characteristic.handle)
          .add("Properties", characteristic.properties)
          .add("Value handle", characteristic.value_handle)
          .add("UUID", characteristic.uuid);
    }

    return builder.build();
  }

  vector<uint8_t> ProbeCharacteristics::serialize(FlatbuffersFormatBuilder& builder) const {
    vector<flatbuffers::Offset<BaBLE::Characteristic>> characteristics;
    characteristics.reserve(m_characteristics.size());

    for (auto& characteristic : m_characteristics) {
      auto uuid = builder.CreateString(AsciiFormat::format_uuid(characteristic.uuid));

      bool indicate = (characteristic.properties & 1 << 5) > 0;
      bool notify = (characteristic.properties & 1 << 4) > 0;
      bool write = (characteristic.properties & 1 << 3) > 0 | (characteristic.properties & 1 << 2) > 0;
      bool read = (characteristic.properties & 1 << 1) > 0;
      bool broadcast = (characteristic.properties & 1) > 0;

      auto characteristic_offset = BaBLE::CreateCharacteristic(
          builder,
          characteristic.handle,
          characteristic.value_handle,
          indicate,
          notify,
          write,
          read,
          broadcast,
          uuid
      );
      characteristics.push_back(characteristic_offset);
    }

    auto characteristics_vector = builder.CreateVector(characteristics);
    auto payload = BaBLE::CreateProbeCharacteristics(builder, m_connection_id, characteristics_vector);

    return builder.build(payload, BaBLE::Payload::ProbeCharacteristics);
  }

  vector<uint8_t> ProbeCharacteristics::serialize(HCIFormatBuilder& builder) const {
    return m_read_by_type_request_packet->serialize(builder);
  }

  void ProbeCharacteristics::before_sent(const std::shared_ptr<PacketRouter>& router) {
    // We want to keep the same Packet::Type to resend command until we probed all the characteristics
    if (m_waiting_characteristics) {
      m_current_type = m_translated_type;

      PacketUuid uuid = get_uuid();
      uuid.packet_code = Format::HCI::AttributeCode::ReadByTypeResponse;
      auto callback = [this](std::shared_ptr<Packet::AbstractPacket> packet) {
        return on_read_by_type_response_received(packet);
      };
      router->add_callback(uuid, callback);
    } else {
      m_current_type = m_initial_type;
    }

    m_packet_code = packet_code(m_current_type);
  }

  shared_ptr<AbstractPacket> ProbeCharacteristics::on_read_by_type_response_received(shared_ptr<AbstractPacket> packet) {
    LOG.debug("Response received", "ProbeCharacteristics");

    if (packet->get_id() == Packet::Id::ErrorResponse) {
      auto error_packet = dynamic_pointer_cast<Packet::Errors::ErrorResponse>(packet);
      if(error_packet == nullptr) {
        throw Exceptions::RuntimeErrorException("Can't downcast AbstractPacket to ErrorResponse packet.");
      }

      Format::HCI::AttributeErrorCode error_code = error_packet->get_error_code();

      if (error_code != Format::HCI::AttributeErrorCode::AttributeNotFound) {
        import_status(*error_packet);
      }

      m_waiting_characteristics = false;

    } else if (packet->get_id() == Packet::Id::ReadByTypeResponse) {
      auto read_by_type_response_packet = dynamic_pointer_cast<Packet::Commands::ReadByTypeResponse>(packet);
      if(read_by_type_response_packet == nullptr) {
        throw Exceptions::RuntimeErrorException("Can't downcast AbstractPacket to ReadByTypeResponse packet.");
      }

      vector<Format::HCI::Characteristic> new_characteristics = read_by_type_response_packet->get_characteristics();

      m_characteristics.insert(m_characteristics.end(), new_characteristics.begin(), new_characteristics.end());

      uint16_t last_ending_handle = read_by_type_response_packet->get_last_ending_handle();
      if (last_ending_handle == 0xFFFF) {
        m_waiting_characteristics = false;
      } else {
        m_waiting_characteristics = true;
        m_read_by_type_request_packet->set_handles(static_cast<uint16_t>(last_ending_handle + 1), 0xFFFF);
      }
    }

    return shared_from_this();
  }

}