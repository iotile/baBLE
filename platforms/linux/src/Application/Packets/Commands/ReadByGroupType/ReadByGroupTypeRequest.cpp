#include "ReadByGroupTypeRequest.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

using namespace std;

namespace Packet::Commands {

  ReadByGroupTypeRequest::ReadByGroupTypeRequest(Packet::Type initial_type, Packet::Type translated_type)
      : RequestPacket(initial_type, translated_type) {
    m_id = Packet::Id::ReadByGroupTypeRequest;
    m_response_packet_code = Format::HCI::AttributeCode::ReadByTypeResponse;

    m_starting_handle = 0x0001;
    m_ending_handle = 0xFFFF;
    m_uuid = static_cast<uint16_t>(Format::HCI::UUID::GattPrimaryServiceDeclaration);
  }

  void ReadByGroupTypeRequest::set_handles(uint16_t starting_handle, uint16_t ending_handle) {
    m_starting_handle = starting_handle;
    m_ending_handle = ending_handle;
  }

  void ReadByGroupTypeRequest::unserialize(AsciiFormatExtractor& extractor) {
    try {
      m_connection_id = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());
      m_starting_handle = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());
      m_ending_handle = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());
      m_uuid = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'ReadByGroupTypeRequest' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,"
                                                "<connection_handle>,<starting_handle>,<ending_handle>,<gatt_uuid>",
                                                m_uuid_request);
    }
  }

  void ReadByGroupTypeRequest::unserialize(HCIFormatExtractor& extractor) {
    m_starting_handle = extractor.get_value<uint16_t>();
    m_ending_handle = extractor.get_value<uint16_t>();
    m_uuid = extractor.get_value<uint16_t>();
  }

  vector<uint8_t> ReadByGroupTypeRequest::serialize(AsciiFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    builder
        .set_name("ReadByGroupType")
        .add("Starting handle", m_starting_handle)
        .add("Ending handle", m_ending_handle)
        .add("GATT UUID", m_uuid);

    return builder.build();
  }

  vector<uint8_t> ReadByGroupTypeRequest::serialize(HCIFormatBuilder& builder) const {
    RequestPacket::serialize(builder);

    builder
        .add(m_starting_handle)
        .add(m_ending_handle)
        .add(m_uuid);

    return builder.build(Format::HCI::Type::AsyncData);
  }

  void ReadByGroupTypeRequest::before_sent(const std::shared_ptr<PacketRouter>& router) {
    RequestPacket::before_sent(router);

    PacketUuid error_uuid = get_uuid();
    error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
    auto error_callback = [this](const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet) {
      return on_error_response_received(router, packet);
    };
    router->add_callback(error_uuid, shared_from(this), error_callback);
  }

  shared_ptr<Packet::AbstractPacket> ReadByGroupTypeRequest::on_response_received(const std::shared_ptr<PacketRouter>& router, const shared_ptr<Packet::AbstractPacket>& packet) {
    LOG.debug("Response received", "ReadByGroupTypeRequest");
    PacketUuid error_uuid = get_uuid();
    error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
    router->remove_callback(error_uuid);

    return RequestPacket::on_response_received(router, packet);
  }

  shared_ptr<Packet::AbstractPacket> ReadByGroupTypeRequest::on_error_response_received(const std::shared_ptr<PacketRouter>& router, const shared_ptr<AbstractPacket>& packet) {
    LOG.debug("ErrorResponse received", "ReadByGroupTypeRequest");
    PacketUuid response_uuid = get_response_uuid();
    router->remove_callback(response_uuid);

    import_status(*packet);

    return shared_from(this);
  }

}
