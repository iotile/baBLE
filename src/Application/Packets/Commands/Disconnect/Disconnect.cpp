#include "Disconnect.hpp"

using namespace std;

namespace Packet::Commands {

  Disconnect::Disconnect(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {
    m_address_type = 0;
  };

  void Disconnect::unserialize(AsciiFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    try {
      m_address_type = static_cast<uint8_t>(stoi(extractor.get()));

      string address_str = extractor.get();
      string item;
      istringstream address_stream(address_str);
      for (uint8_t& item_address : m_address) {
        if (!getline(address_stream, item, ':')) {
          throw Exceptions::InvalidCommandException("Can't parse <address> argument for 'Disconnect' packet.", m_uuid_request);
        }
        item_address = static_cast<uint8_t>(stoi(item));
      }

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Missing arguments for 'Disconnect' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,<address_type>,<address>", m_uuid_request);
    } catch (const bad_cast& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'Disconnect' packet. Can't cast", m_uuid_request);
    } catch (const std::invalid_argument& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'Disconnect' packet.", m_uuid_request);
    }
  };

  void Disconnect::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
    auto payload = extractor.get_payload<const Schemas::Disconnect*>(Schemas::Payload::Disconnect);

    m_address_type = payload->address_type();

    auto raw_fb_address = payload->address();
    if (raw_fb_address->size() != m_address.size()) {
      throw Exceptions::InvalidCommandException("Given address is not valid: size different from expected.", m_uuid_request);
    }
    copy(raw_fb_address->begin(), raw_fb_address->end(), m_address.begin());
  };

  void Disconnect::unserialize(MGMTFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    if (m_native_status == 0){
      m_address = extractor.get_array<uint8_t, 6>();
      m_address_type = extractor.get_value<uint8_t>();
    }
  };

  vector<uint8_t> Disconnect::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
        .set_name("Disconnect")
        .add("Address type", m_address_type)
        .add("Address", m_address);

    return builder.build();
  };

  vector<uint8_t> Disconnect::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    auto address = builder.CreateVector(m_address.data(), m_address.size());

    auto payload = Schemas::CreateDisconnect(builder, address, m_address_type);

    return builder.build(payload, Schemas::Payload::Disconnect);
  }

  vector<uint8_t> Disconnect::serialize(MGMTFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    builder
        .add(m_address)
        .add(m_address_type);
    return builder.build();
  }

}
