#ifndef BABLE_LINUX_DISCOVERING_HPP
#define BABLE_LINUX_DISCOVERING_HPP

#include "../EventPacket.hpp"

namespace Packet::Events {

  class Discovering : public EventPacket<Discovering> {

  public:
    static const uint16_t event_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Events::MGMT::Code::Discovering;

        case Packet::Type::ASCII:
          return Events::Ascii::Code::Discovering;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::Discovering);

        default:
          throw std::runtime_error("Current type has no known id (Discovering).");
      }
    };

    Discovering(Packet::Type initial_type, Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_address_type = 0;
      m_discovering = 0;
    };

    void import(MGMTFormatExtractor& extractor) override {
      EventPacket::import(extractor);
      m_address_type = extractor.get_value<uint8_t>();
      m_discovering = extractor.get_value<uint8_t>();
    };

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override {
      EventPacket::serialize(builder);
      builder
          .set_name("Discovering")
          .add("Address type", m_address_type)
          .add("Discovering", m_discovering);

      return builder.build();
    };

    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override {
      EventPacket::serialize(builder);
      auto payload = Schemas::CreateDiscovering(builder, m_controller_id, m_address_type, m_discovering);

      return builder.build(payload, Schemas::Payload::Discovering);
    }

  private:
    uint8_t m_address_type;
    uint8_t m_discovering;
  };

}

#endif //BABLE_LINUX_DISCOVERING_HPP
