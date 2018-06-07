#include "CommandComplete.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    CommandComplete::CommandComplete()
        : ControllerToHostPacket(Packet::Id::CommandComplete, initial_type(), initial_packet_code(), final_packet_code(), true) {
      m_opcode = 0;
    }

    void CommandComplete::unserialize(HCIFormatExtractor& extractor) {
      auto number_allowed_command_packets = extractor.get_value<uint8_t>();
      m_opcode = extractor.get_value<uint16_t>();
      m_returned_params = extractor.get_vector<uint8_t>();
    }

    const std::string CommandComplete::stringify() const {
      stringstream result;

      result << "<CommandComplete> "
             << AbstractPacket::stringify() << ", "
             << "Opcode: " << to_string(m_opcode) << ", "
             << "Returned parameters: " << Utils::format_bytes_array(m_returned_params);

      return result.str();
    }

    const PacketUuid CommandComplete::get_uuid() const {
        return PacketUuid{
        m_current_type,
        m_controller_id,
        m_connection_id,
        m_packet_code,
        get_opcode()
      };
    }

  }

}