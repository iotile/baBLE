#include "ProbeServices.hpp"
#include "../../Commands/ReadByGroupType/ReadByGroupTypeResponse.hpp"
#include "../../../../Exceptions/RuntimeError/RuntimeErrorException.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

using namespace std;

namespace Packet::Meta {

  ProbeServices::ProbeServices(Packet::Type initial_type, Packet::Type translated_type)
      : AbstractPacket(initial_type, translated_type) {
    m_id = Packet::Id::ProbeServices;
    m_packet_code = packet_code(m_current_type);

    m_waiting_services = true;
    m_read_by_type_group_request_packet = make_shared<Packet::Commands::ReadByGroupTypeRequest>(translated_type, translated_type);
  }

  void ProbeServices::unserialize(AsciiFormatExtractor& extractor) {
    try {
      m_connection_id = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'ProbeServices' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,<connection_handle>", m_uuid_request);
    }
  }

  void ProbeServices::unserialize(FlatbuffersFormatExtractor& extractor) {
    auto payload = extractor.get_payload<const BaBLE::ProbeServices*>();

    m_connection_id = payload->connection_handle();
  }

  vector<uint8_t> ProbeServices::serialize(AsciiFormatBuilder& builder) const {
    builder
        .set_name("ProbeServices")
        .add("Type", "Meta")
        .add("Connection id", m_connection_id);

    for (auto& service : m_services) {
      builder
          .add("Handle", service.handle)
          .add("Group end handle", service.group_end_handle)
          .add("UUID", service.uuid);
    }

    return builder.build();
  }

  vector<uint8_t> ProbeServices::serialize(FlatbuffersFormatBuilder& builder) const {
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
    auto payload = BaBLE::CreateProbeServices(builder, m_connection_id, services_vector);

    return builder.build(payload, BaBLE::Payload::ProbeServices);
  }

  vector<uint8_t> ProbeServices::serialize(HCIFormatBuilder& builder) const {
    return m_read_by_type_group_request_packet->serialize(builder);
  }

  void ProbeServices::before_sent(const std::shared_ptr<PacketRouter>& router) {
    // We want to keep the same Packet::Type to resend command until we probed all the services
    if (m_waiting_services) {
      m_current_type = m_translated_type;

      PacketUuid response_uuid = get_uuid();
      response_uuid.response_packet_code = Format::HCI::AttributeCode::ReadByGroupTypeResponse;
      auto response_callback = [this](const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet) {
        return on_read_by_group_type_response_received(router, packet);
      };

      PacketUuid error_uuid = get_uuid();
      error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
      auto error_callback = [this](const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet) {
        return on_error_response_received(router, packet);
      };

      router->add_callback(response_uuid, shared_from(this), response_callback);
      router->add_callback(error_uuid, shared_from(this), error_callback);
    } else {
      m_current_type = m_initial_type;
      m_packet_code = packet_code(m_current_type);
    }
  }

  shared_ptr<AbstractPacket> ProbeServices::on_read_by_group_type_response_received(const std::shared_ptr<PacketRouter>& router, const shared_ptr<AbstractPacket>& packet) {
    LOG.debug("Response received", "ProbeServices");
    PacketUuid error_uuid = get_uuid();
    error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
    router->remove_callback(error_uuid);

    auto read_by_group_type_response_packet = dynamic_pointer_cast<Packet::Commands::ReadByGroupTypeResponse>(packet);
    if(read_by_group_type_response_packet == nullptr) {
      throw Exceptions::RuntimeErrorException("Can't downcast AbstractPacket to ReadByGroupTypeResponse packet.");
    }

    vector<Format::HCI::Service> new_services = read_by_group_type_response_packet->get_services();

    m_services.insert(m_services.end(), new_services.begin(), new_services.end());

    uint16_t last_group_end_handle = read_by_group_type_response_packet->get_last_group_end_handle();
    if (last_group_end_handle == 0xFFFF) {
      m_waiting_services = false;
    } else {
      m_waiting_services = true;
      m_read_by_type_group_request_packet->set_handles(static_cast<uint16_t>(last_group_end_handle + 1), 0xFFFF);
    }

    return shared_from(this);
  }

  shared_ptr<AbstractPacket> ProbeServices::on_error_response_received(const std::shared_ptr<PacketRouter>& router, const shared_ptr<AbstractPacket>& packet) {
    LOG.debug("Error received", "ProbeCharacteristics");
    PacketUuid response_uuid = get_uuid();
    response_uuid.response_packet_code = Format::HCI::AttributeCode::ReadByTypeResponse;
    router->remove_callback(response_uuid);

    import_status(*packet);

    return shared_from(this);
  }

}