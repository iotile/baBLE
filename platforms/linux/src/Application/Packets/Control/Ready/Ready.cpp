#include <sstream>
#include "Ready.hpp"

using namespace std;

namespace Packet {

  namespace Control {

    Ready::Ready()
        : HostOnlyPacket(Packet::Id::Ready, initial_packet_code()) {}

    vector<uint8_t> Ready::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateReady(builder);

      return builder.build(payload, BaBLE::Payload::Ready);
    }

    const string Ready::stringify() const {
      stringstream result;

      result << "<Ready> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}