#ifndef BABLE_LINUX_COMMANDPACKET_HPP
#define BABLE_LINUX_COMMANDPACKET_HPP

#include "../../AbstractPacket.hpp"

namespace Packet::Commands {

  template<class T>
  class CommandPacket : public AbstractPacket {

  protected:
    CommandPacket(Packet::Type initial_type, Packet::Type translated_type)
        : AbstractPacket(initial_type, translated_type) {
      m_command_code = T::packet_code(current_type());
      m_response_received = false;
    }

    void after_translate() override {
      m_command_code = T::packet_code(current_type());
    }

    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override {
      builder.set_code(m_command_code);

      return {};
    }

    std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override {
      builder.set_opcode(static_cast<uint8_t>(m_command_code));

      return {};
    }

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override {
      builder
          .add("Type", "Command")
          .add("Command code", m_command_code);

      return {};
    }

    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override {
      return {};
    }

    void unserialize(MGMTFormatExtractor& extractor) override {
      m_command_code = extractor.get_value<uint16_t>();
      m_native_status = extractor.get_value<uint8_t>();
      compute_bable_status();
    }

    void unserialize(HCIFormatExtractor& extractor) override {}

    void unserialize(AsciiFormatExtractor& extractor) override {
      m_command_code = extractor.get_command_code();
    }

    void unserialize(FlatbuffersFormatExtractor& extractor) override {}

    uint64_t expected_response_uuid() override {
      if (!m_response_received) {
        return Packet::AbstractPacket::compute_uuid(m_controller_id, T::packet_code(m_translated_type));

      } else {
        return 0;
      }
    }

    bool on_response_received(Packet::Type packet_type, const std::vector<uint8_t>& raw_data) override {
      if (packet_type != m_translated_type) {
        return false;
      }

      LOG.debug("Response received", "CommandPacket");
      from_bytes(raw_data, m_controller_id);
      m_response_received = true;
      return true;
    }

    uint16_t m_command_code;
    bool m_response_received;

  };

}

#endif //BABLE_LINUX_COMMANDPACKET_HPP