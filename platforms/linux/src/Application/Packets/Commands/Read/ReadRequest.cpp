#include "ReadRequest.hpp"

using namespace std;

namespace Packet::Commands {

  ReadRequest::ReadRequest(Packet::Type initial_type, Packet::Type translated_type)
      : RequestPacket(initial_type, translated_type) {
    m_id = Packet::Id::ReadRequest;
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
    AbstractPacket::before_sent(router);
    m_packet_code = packet_code(m_current_type);

    PacketUuid response_uuid = get_uuid();
    response_uuid.packet_code = Format::HCI::AttributeCode::ReadResponse;

    auto callback = [this](std::shared_ptr<Packet::AbstractPacket> packet) {
      return on_read_response_received(packet);
    };
    router->add_callback(response_uuid, callback);
  }

  shared_ptr<Packet::AbstractPacket> ReadRequest::on_read_response_received(shared_ptr<Packet::AbstractPacket> packet) {
    LOG.debug("Response received", "ReadRequest");
    auto read_response_packet = dynamic_pointer_cast<Packet::Commands::ReadResponse>(packet);
    if(read_response_packet == nullptr) {
      throw Exceptions::RuntimeErrorException("Can't downcast AbstractPacket to ReadResponse packet.");
    }

    read_response_packet->set_uuid_request(m_uuid_request);
    read_response_packet->set_attribute_handle(m_attribute_handle);

    return move(read_response_packet);
  }

}