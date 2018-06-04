#include "Discovering.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    Discovering::Discovering(Packet::Type initial_type, Packet::Type final_type)
        : EventPacket(initial_type, final_type) {
      m_id = Packet::Id::Discovering;
      m_address_type = 0;
      m_discovering = false;
    }

    void Discovering::unserialize(MGMTFormatExtractor& extractor) {
      m_address_type = extractor.get_value<uint8_t>();
      m_discovering = extractor.get_value<uint8_t>() == 1;
    }

    vector<uint8_t> Discovering::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateDiscovering(builder, m_address_type, m_discovering);

      return builder.build(payload, BaBLE::Payload::Discovering);
    }

    const std::string Discovering::stringify() const {
      stringstream result;

      result << "<Discovering> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_discovering) << ", "
             << "Address type: " << to_string(m_address_type);

      return result.str();
    }

  }

}
