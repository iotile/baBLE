#include "ReadByGroupTypeRequest.hpp"

using namespace std;

namespace Packet::Commands {

  ReadByGroupTypeRequest::ReadByGroupTypeRequest(Packet::Type initial_type, Packet::Type translated_type)
      : RequestPacket(initial_type, translated_type) {
    m_id = Packet::Id::ReadByGroupTypeRequest;
    m_starting_handle = 0x0001;
    m_ending_handle = 0xFFFF;
    m_uuid = static_cast<uint16_t>(Format::HCI::UUID::GattPrimaryServiceDeclaration);
  }

  ReadByGroupTypeRequest& ReadByGroupTypeRequest::set_handles(uint16_t starting_handle, uint16_t ending_handle) {
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

}