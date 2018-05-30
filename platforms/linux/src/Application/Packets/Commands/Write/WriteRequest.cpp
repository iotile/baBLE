#include "WriteRequest.hpp"

using namespace std;

namespace Packet::Commands {

  WriteRequest::WriteRequest(Packet::Type initial_type, Packet::Type translated_type)
      : RequestPacket(initial_type, translated_type) {
    m_id = Packet::Id::WriteRequest;
    m_attribute_handle = 0;
  }

  void WriteRequest::unserialize(AsciiFormatExtractor& extractor) {
    try {
      m_connection_id = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());
      m_attribute_handle = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());
      m_data_to_write = extractor.get_vector<uint8_t>();

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'Write' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,"
                                                       "<connection_handle>,<attribute_handle>,<data>", m_uuid_request);
    }
  }

  void WriteRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
    auto payload = extractor.get_payload<const BaBLE::Write*>();

    m_connection_id = payload->connection_handle();
    m_attribute_handle = payload->attribute_handle();
    auto raw_data_to_write = payload->value();
    m_data_to_write.assign(raw_data_to_write->begin(), raw_data_to_write->end());
  }

  vector<uint8_t> WriteRequest::serialize(AsciiFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    builder
        .set_name("Write")
        .add("Attribute handle", m_attribute_handle)
        .add("Data to write", m_data_to_write);

    return builder.build();
  }

  vector<uint8_t> WriteRequest::serialize(HCIFormatBuilder& builder) const {
    RequestPacket::serialize(builder);

    builder
        .add(m_attribute_handle)
        .add(m_data_to_write);

    return builder.build(Format::HCI::Type::AsyncData);
  }

  void WriteRequest::before_sent(const std::shared_ptr<PacketRouter>& router) {
    AbstractPacket::before_sent(router);
    m_packet_code = packet_code(m_current_type);

    PacketUuid response_uuid = get_uuid();
    response_uuid.packet_code = Format::HCI::AttributeCode::WriteResponse;

    auto callback = [this](std::shared_ptr<Packet::AbstractPacket> packet) {
      return on_write_response_received(packet);
    };
    router->add_callback(response_uuid, callback);
  }

  shared_ptr<Packet::AbstractPacket> WriteRequest::on_write_response_received(shared_ptr<Packet::AbstractPacket> packet) {
    LOG.debug("Response received", "WriteRequest");
    auto write_response_packet = dynamic_pointer_cast<Packet::Commands::WriteResponse>(packet);
    if(write_response_packet == nullptr) {
      throw Exceptions::RuntimeErrorException("Can't downcast AbstractPacket to WriteResponse packet.");
    }

    write_response_packet->set_uuid_request(m_uuid_request);
    write_response_packet->set_attribute_handle(m_attribute_handle);

    return move(write_response_packet);
  }

}