#include "ReadRequest.hpp"

using namespace std;

namespace Packet::Commands {

  ReadRequest::ReadRequest(Packet::Type initial_type, Packet::Type translated_type)
      : RequestPacket(initial_type, translated_type) {
    m_id = Packet::Id::ReadRequest;
    m_response_packet_code = Format::HCI::AttributeCode::ReadResponse;

    m_attribute_handle = 0;
  }

  void ReadRequest::unserialize(AsciiFormatExtractor& extractor) {
    try {
      m_connection_id = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());
      m_attribute_handle = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'Read' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,"
                                                       "<connection_handle>,<attribute_handle>", m_uuid_request);
    }
  }

  void ReadRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
    auto payload = extractor.get_payload<const BaBLE::Read*>();

    m_connection_id = payload->connection_handle();
    m_attribute_handle = payload->attribute_handle();
  }

  void ReadRequest::unserialize(HCIFormatExtractor& extractor) {
    m_attribute_handle = extractor.get_value<uint16_t>();
  }

  vector<uint8_t> ReadRequest::serialize(AsciiFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    builder
        .set_name("Read")
        .add("Attribute handle", m_attribute_handle);

    return builder.build();
  }

  vector<uint8_t> ReadRequest::serialize(HCIFormatBuilder& builder) const {
    RequestPacket::serialize(builder);

    builder.add(m_attribute_handle);

    return builder.build(Format::HCI::Type::AsyncData);
  }

  void ReadRequest::before_sent(const std::shared_ptr<PacketRouter>& router) {
    RequestPacket::before_sent(router);

    PacketUuid error_uuid = get_uuid();
    error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
    auto error_callback = [this](const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet) {
      return on_error_response_received(router, packet);
    };
    router->add_callback(error_uuid, shared_from(this), error_callback);
  }

  shared_ptr<Packet::AbstractPacket> ReadRequest::on_response_received(const std::shared_ptr<PacketRouter>& router, const shared_ptr<Packet::AbstractPacket>& packet) {
    LOG.debug("Response received", "ReadRequest");
    PacketUuid error_uuid = get_uuid();
    error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
    router->remove_callback(error_uuid);

    auto read_response_packet = dynamic_pointer_cast<Packet::Commands::ReadResponse>(packet);
    if(read_response_packet == nullptr) {
      throw Exceptions::RuntimeErrorException("Can't downcast AbstractPacket to ReadResponse packet.");
    }

    read_response_packet->set_uuid_request(m_uuid_request);
    read_response_packet->set_attribute_handle(m_attribute_handle);

    return read_response_packet;
  }

  shared_ptr<Packet::AbstractPacket> ReadRequest::on_error_response_received(const std::shared_ptr<PacketRouter>& router, const shared_ptr<AbstractPacket>& packet) {
    LOG.debug("ErrorResponse received", "ReadRequest");
    PacketUuid response_uuid = get_response_uuid();
    router->remove_callback(response_uuid);

    import_status(*packet);

    return shared_from(this);
  }

}