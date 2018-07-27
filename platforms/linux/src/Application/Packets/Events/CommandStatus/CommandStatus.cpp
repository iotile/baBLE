#include <sstream>
#include "CommandStatus.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    CommandStatus::CommandStatus()
        : ControllerToHostPacket(Packet::Id::CommandStatus, initial_type(), initial_packet_code(), final_packet_code(), true) {
      m_opcode = 0;
    }

    void CommandStatus::unserialize(HCIFormatExtractor& extractor) {
      set_status(extractor.get_value<uint8_t>());
      auto number_allowed_command_packets = extractor.get_value<uint8_t>();
      m_opcode = extractor.get_value<uint16_t>();
    }

    const string CommandStatus::stringify() const {
      stringstream result;

      result << "<CommandStatus> "
             << AbstractPacket::stringify() << ", "
             << "Opcode: " << to_string(m_opcode);

      return result.str();
    }

    const PacketUuid CommandStatus::get_uuid() const {
      return PacketUuid{
          m_current_type,
          m_controller_id,
          m_connection_handle,
          m_packet_code,
          get_opcode()
      };
    }

  }

}