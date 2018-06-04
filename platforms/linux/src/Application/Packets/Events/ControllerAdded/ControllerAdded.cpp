#include "ControllerAdded.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    ControllerAdded::ControllerAdded(Packet::Type initial_type, Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_id = Packet::Id::ControllerAdded;
    }

    void ControllerAdded::unserialize(MGMTFormatExtractor& extractor) {}

    vector<uint8_t> ControllerAdded::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateControllerAdded(builder);

      return builder.build(payload, BaBLE::Payload::ControllerAdded);
    }

    const std::string ControllerAdded::stringify() const {
      stringstream result;

      result << "<ControllerAdded> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}