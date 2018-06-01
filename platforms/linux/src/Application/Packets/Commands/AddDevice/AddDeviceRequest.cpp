#include "AddDeviceRequest.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

using namespace std;

namespace Packet::Commands {

  AddDeviceRequest::AddDeviceRequest(Packet::Type initial_type, Packet::Type translated_type)
      : RequestPacket(initial_type, translated_type) {
    m_id = Packet::Id::AddDeviceRequest;
    m_address_type = 0;
    m_action = 2;
  }

  void AddDeviceRequest::unserialize(AsciiFormatExtractor& extractor) {
    try {
      m_address_type = AsciiFormat::string_to_number<uint8_t>(extractor.get_string());
      m_address = extractor.get_string();

      string item;
      istringstream address_stream(m_address);
      for (auto it = m_raw_address.rbegin(); it != m_raw_address.rend(); ++it) {
        if (!getline(address_stream, item, ':')) {
          throw Exceptions::InvalidCommandException("Can't parse <address> argument for 'AddDevice' packet.", m_uuid_request);
        }
        *it = AsciiFormat::string_to_number<uint8_t>(item, 16);
      }

      m_action = AsciiFormat::string_to_number<uint8_t>(extractor.get_string());

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'AddDevice' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,"
                                                       "<address_type>,<address>,<action>", m_uuid_request);
    }
  }

  void AddDeviceRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
    auto payload = extractor.get_payload<const BaBLE::AddDevice*>();

    m_address_type = payload->address_type();
    m_address = payload->address()->str();

    string item;
    istringstream address_stream(m_address);
    for (auto it = m_raw_address.rbegin(); it != m_raw_address.rend(); ++it) {
      if (!getline(address_stream, item, ':')) {
        throw Exceptions::InvalidCommandException("Can't parse 'address' argument for 'AddDevice' packet.", m_uuid_request);
      }
      *it = AsciiFormat::string_to_number<uint8_t>(item, 16);
    }
  }

  vector<uint8_t> AddDeviceRequest::serialize(AsciiFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    builder
        .set_name("AddDevice")
        .add("Address type", m_address_type)
        .add("Address", m_address);

    return builder.build();
  }

  vector<uint8_t> AddDeviceRequest::serialize(MGMTFormatBuilder& builder) const {
    RequestPacket::serialize(builder);

    builder
        .add(m_raw_address)
        .add(m_address_type)
        .add(m_action);
    return builder.build();
  }

}
