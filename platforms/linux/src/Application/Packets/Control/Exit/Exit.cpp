#include <sstream>
#include "Exit.hpp"

using namespace std;

namespace Packet {

  namespace Control {

    Exit::Exit()
        : HostOnlyPacket(Packet::Id::Exit, initial_packet_code()) {
      m_native_class = "BaBLE";
    }

    void Exit::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> Exit::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateExit(builder);

      return builder.build(payload, BaBLE::Payload::Exit);
    }

    const string Exit::stringify() const {
      stringstream result;

      result << "<Exit> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}