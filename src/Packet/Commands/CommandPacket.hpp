#ifndef BABLE_LINUX_COMMANDPACKET_HPP
#define BABLE_LINUX_COMMANDPACKET_HPP

#include "../AbstractPacket.hpp"

namespace Packet::Commands {

  template<class T>
  class CommandPacket : public AbstractPacket {

  protected:
    CommandPacket(Packet::Type initial_type, Packet::Type translated_type)
        : AbstractPacket(initial_type, translated_type) {
      m_command_code = T::command_code(m_current_type);
      m_controller_id = 0xFFFF;
      m_status = 0;
    };

    void after_translate() override {
      m_command_code = T::command_code(m_current_type);
    };

    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override {
      builder.set_code(m_command_code);

      return {};
    };

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override {
      builder
          .add("Type", "Command")
          .add("Command code", m_command_code)
          .add("Status", m_status);

      return {};
    };

    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override {
      return {};
    };

    void import(MGMTFormatExtractor& extractor) override {
      m_command_code = extractor.get_value<uint16_t>();
      m_status = extractor.get_value<uint8_t>();
    };

    void import(AsciiFormatExtractor& extractor) override {
      m_command_code = static_cast<uint16_t>(stoi(extractor.get()));
    };
    void import(FlatbuffersFormatExtractor& extractor) override {};

    uint16_t m_command_code;
    uint8_t m_status;

  };

}

#endif //BABLE_LINUX_COMMANDPACKET_HPP