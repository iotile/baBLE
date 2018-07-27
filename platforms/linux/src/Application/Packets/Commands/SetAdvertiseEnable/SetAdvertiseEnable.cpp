#include "SetAdvertiseEnable.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetAdvertiseEnable::SetAdvertiseEnable(bool state)
        : HostToControllerPacket(Packet::Id::SetAdvertiseEnable, final_type(), final_packet_code()) {
      m_response_packet_code = Format::HCI::EventCode::CommandComplete;

      m_state = state;
    }

    vector<uint8_t> SetAdvertiseEnable::serialize(HCIFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);

      builder.add(m_state);

      return builder.build(Format::HCI::Type::Command);
    }

    void SetAdvertiseEnable::set_state(bool state) {
      m_state = state;
    }

    const string SetAdvertiseEnable::stringify() const {
      stringstream result;

      result << "<SetAdvertiseEnable> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_state);

      return result.str();
    }

  }

}
