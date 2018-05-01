#ifndef BABLE_LINUX_COMMANDS_STARTSCAN_HPP
#define BABLE_LINUX_COMMANDS_STARTSCAN_HPP

#include "../CommandPacket.hpp"

namespace Packet::Commands {

  class StartScan : public CommandPacket<StartScan> {

  public:
    static const uint16_t command_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Commands::MGMT::Code::StartScan;

        case Packet::Type::ASCII:
          return Commands::Ascii::Code::StartScan;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::StartScan);

        default:
          throw std::runtime_error("Current type has no known id (StartScan).");
      }
    };

    StartScan(Packet::Type initial_type, Packet::Type translated_type)
        : CommandPacket(initial_type, translated_type) {
      m_address_type = 0;
    };

    void import(AsciiFormatExtractor& extractor) override {
      CommandPacket::import(extractor);

      try {
        m_controller_id = static_cast<uint16_t>(stoi(extractor.get()));
        m_address_type = static_cast<uint8_t>(stoi(extractor.get()));

      } catch (const std::runtime_error& err) {
        throw std::runtime_error("Missing arguments for 'StartScan' packet. Usage: <command_code>,<controller_id>,<address_type>");
      }
    };

    void import(FlatbuffersFormatExtractor& extractor) override {
      CommandPacket::import(extractor);
      auto payload = extractor.get_payload<const Schemas::StartScan*>(Schemas::Payload::StartScan);

      m_controller_id = payload->controller_id();
      m_address_type = payload->address_type();
    };

    void import(MGMTFormatExtractor& extractor) override {
      CommandPacket::import(extractor);
      m_address_type = extractor.get_value<uint8_t>();
    };

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override {
      CommandPacket::serialize(builder);
      builder
          .set_name("StartScan")
          .add("Address type", m_address_type);

      return builder.build();
    };

    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override {
      CommandPacket::serialize(builder);
      auto payload = Schemas::CreateStartScan(builder, m_controller_id, m_address_type);

      return builder.build(payload, Schemas::Payload::StartScan);
    }

    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override {
      CommandPacket::serialize(builder);
      builder.add(m_address_type);
      return builder.build();
    };

  private:
    uint8_t m_address_type;

  };

}

#endif //BABLE_LINUX_COMMANDS_STARTSCAN_HPP
