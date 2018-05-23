#include "ProbeServices.hpp"

using namespace std;

namespace Packet::Commands {

  ProbeServices::ProbeServices(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {
    m_id = BaBLE::Payload::ProbeServices;
    m_starting_handle = 0x0001;
    m_ending_handle = 0xFFFF;
    m_waiting_services = true;
  }

  void ProbeServices::unserialize(AsciiFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    try {
      m_connection_handle = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'ProbeServices' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,<connection_handle>", m_uuid_request);
    }
  }

  void ProbeServices::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
    auto payload = extractor.get_payload<const BaBLE::ProbeServices*>();

    m_connection_handle = payload->connection_handle();
  }

  void ProbeServices::unserialize(HCIFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    uint16_t data_length = extractor.get_data_length();

    if (data_length <= 0) {
      throw Exceptions::WrongFormatException("Receive HCI 'ProbeServices' response with no data.");
    }

    auto attribute_length = extractor.get_value<uint8_t>();
    data_length -= 1; // To remove attribute_length from remaining data_length

    size_t num_services = data_length / attribute_length;
    m_services.reserve(m_services.size() + num_services);

    while(data_length >= attribute_length) {
      Format::HCI::Service service{};
      service.handle = extractor.get_value<uint16_t>();
      service.group_end_handle = extractor.get_value<uint16_t>();
      service.uuid = extractor.get_vector<uint8_t>(attribute_length - sizeof(service.handle) - sizeof(service.group_end_handle));

      m_services.push_back(service);
      m_ending_handle = service.group_end_handle;
      data_length -= attribute_length;
    }

    if (m_ending_handle == 0xFFFF) {
      m_waiting_services = false;
    } else {
      m_waiting_services = true;
      m_starting_handle = static_cast<uint16_t>(m_ending_handle + 1);
      m_ending_handle = 0xFFFF;
    }
  }

  vector<uint8_t> ProbeServices::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
        .set_name("ProbeServices")
        .add("Connection handle", m_connection_handle)
        .add("Starting handle", m_starting_handle)
        .add("Ending handle", m_ending_handle);

    for (auto& service : m_services) {
      builder
          .add("Handle", service.handle)
          .add("Group end handle", service.group_end_handle)
          .add("UUID", service.uuid);
    }

    return builder.build();
  }

  vector<uint8_t> ProbeServices::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    std::vector<flatbuffers::Offset<BaBLE::Service>> services;
    services.reserve(m_services.size());

    for (auto& service : m_services) {
      auto uuid = builder.CreateString(AsciiFormat::format_uuid(service.uuid));

      auto service_offset = BaBLE::CreateService(
          builder,
          service.handle,
          service.group_end_handle,
          uuid
      );
      services.push_back(service_offset);
    }

    auto services_vector = builder.CreateVector(services);
    auto payload = BaBLE::CreateProbeServices(builder, m_connection_handle, services_vector);

    return builder.build(payload, BaBLE::Payload::ProbeServices);
  }

  vector<uint8_t> ProbeServices::serialize(HCIFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    auto uuid = static_cast<uint16_t>(Format::HCI::UUID::GattPrimaryServiceDeclaration);
    builder
        .set_connection_handle(m_connection_handle)
        .add(m_starting_handle)
        .add(m_ending_handle)
        .add(uuid);

    return builder.build(Format::HCI::Type::AsyncData);
  }

  void ProbeServices::translate() {
    if (!m_response_received | !m_waiting_services) {
      AbstractPacket::translate();
    }
  }

  vector<ResponseId> ProbeServices::expected_response_ids() {
    if (!m_response_received | m_waiting_services) {
      return {
        ResponseId{m_current_type, m_controller_id, m_connection_handle, Format::HCI::AttributeCode::ReadByGroupTypeResponse}
      };

    } else {
      return {};
    }
  }

}