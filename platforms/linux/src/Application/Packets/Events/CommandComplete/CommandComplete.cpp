#include "CommandComplete.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    CommandComplete::CommandComplete(Packet::Type initial_type, Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_id = Packet::Id::CommandComplete;
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

  }

}