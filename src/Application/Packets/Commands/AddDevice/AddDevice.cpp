#include "AddDevice.hpp"

using namespace std;

namespace Packet::Commands {

  AddDevice::AddDevice(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {
    m_address_type = 0;
  }

  void AddDevice::unserialize(AsciiFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    try {
      m_address_type = AsciiFormat::string_to_number<uint8_t>(extractor.get_string());

      string address_str = extractor.get_string();
      string item;
      istringstream address_stream(address_str);
      for (uint8_t& item_address : m_address) {
        if (!getline(address_stream, item, ':')) {
          throw Exceptions::InvalidCommandException("Can't parse <address> argument for 'AddDevice' packet.", m_uuid_request);
        }
        item_address = AsciiFormat::string_to_number<uint8_t>(item);
      }

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'AddDevice' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,<address_type>,<address>", m_uuid_request);
    }
  }

  void AddDevice::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
    auto payload = extractor.get_payload<const Schemas::AddDevice*>();

    m_address_type = payload->address_type();

    auto raw_fb_address = payload->address();
    if (raw_fb_address->size() != m_address.size()) {
      throw Exceptions::InvalidCommandException("Given address is not valid: size different from expected.", m_uuid_request);
    }
    copy(raw_fb_address->begin(), raw_fb_address->end(), m_address.begin());
  }

  void AddDevice::unserialize(MGMTFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    if (m_status == Schemas::StatusCode::Success){
      m_address = extractor.get_array<uint8_t, 6>();
      m_address_type = extractor.get_value<uint8_t>();
    }
  }

  vector<uint8_t> AddDevice::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
        .set_name("AddDevice")
        .add("Address type", m_address_type)
        .add("Address", m_address);

    return builder.build();
  }

  vector<uint8_t> AddDevice::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    auto address = builder.CreateVector(m_address.data(), m_address.size());

    auto payload = Schemas::CreateAddDevice(builder, address, m_address_type);

    return builder.build(payload, Schemas::Payload::AddDevice);
  }

  vector<uint8_t> AddDevice::serialize(MGMTFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    uint8_t action = 2;

    builder
        .add(m_address)
        .add(m_address_type)
        .add(action);
    return builder.build();
  }

}
