#include <sstream>
#include "ControllerRemoved.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    ControllerRemoved::ControllerRemoved()
        : ControllerToHostPacket(Packet::Id::ControllerRemoved, initial_type(), initial_packet_code(), final_packet_code()) {
    }

    void ControllerRemoved::unserialize(MGMTFormatExtractor& extractor) {}

    vector<uint8_t> ControllerRemoved::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateControllerRemoved(builder);

      return builder.build(payload, BaBLE::Payload::ControllerRemoved);
    }

    const string ControllerRemoved::stringify() const {
      stringstream result;

      result << "<ControllerRemoved> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}