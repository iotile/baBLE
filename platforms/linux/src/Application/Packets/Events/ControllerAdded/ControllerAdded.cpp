#include <sstream>
#include "ControllerAdded.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    ControllerAdded::ControllerAdded()
        : ControllerToHostPacket(Packet::Id::ControllerAdded,initial_type(), initial_packet_code(), final_packet_code()) {
    }

    void ControllerAdded::unserialize(MGMTFormatExtractor& extractor) {}

    vector<uint8_t> ControllerAdded::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateControllerAdded(builder);

      return builder.build(payload, BaBLE::Payload::ControllerAdded);
    }

    const string ControllerAdded::stringify() const {
      stringstream result;

      result << "<ControllerAdded> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}