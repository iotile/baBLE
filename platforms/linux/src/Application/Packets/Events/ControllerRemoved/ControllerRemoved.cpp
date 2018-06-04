#include "ControllerRemoved.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    ControllerRemoved::ControllerRemoved(Packet::Type initial_type, Packet::Type final_type)
        : EventPacket(initial_type, final_type) {
      m_id = Packet::Id::ControllerRemoved;
    }

    void ControllerRemoved::unserialize(MGMTFormatExtractor& extractor) {}

    vector<uint8_t> ControllerRemoved::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateControllerRemoved(builder);

      return builder.build(payload, BaBLE::Payload::ControllerRemoved);
    }

    const std::string ControllerRemoved::stringify() const {
      stringstream result;

      result << "<ControllerRemoved> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}