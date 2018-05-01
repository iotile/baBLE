#ifndef BABLE_LINUX_DISCOVERING_HPP
#define BABLE_LINUX_DISCOVERING_HPP

#include <cstdint>
#include <flatbuffers/flatbuffers.h>
#include <Packet_generated.h>
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

    Discovering(Packet::Type initial_type, Packet::Type translated_type): EventPacket(initial_type, translated_type) {
      m_address_type = 0;
      m_discovering = 0;
    };

    void from_mgmt(Deserializer& deser) override {
      EventPacket::from_mgmt(deser);
      deser >> m_address_type >> m_discovering;
    };

    std::string to_ascii() const override {
      std::string header = EventPacket::to_ascii();
      std::stringstream payload;

      payload << "Address type: " << std::to_string(m_address_type) << ", "
              << "Discovering: " << std::to_string(m_discovering);

      return header + ", " + payload.str();
    };

    Serializer to_flatbuffers() const override {
      flatbuffers::FlatBufferBuilder builder(0);

      auto payload = Schemas::CreateDiscovering(builder, m_controller_id, m_address_type, m_discovering);

      Serializer ser = build_flatbuffers_packet<Schemas::Discovering>(builder, payload, Schemas::Payload::Discovering);

      Serializer result;
      result << static_cast<uint8_t>(0xCA) << static_cast<uint8_t>(0xFE) << static_cast<uint16_t>(ser.size()) << ser;

      return result;
    }

  private:
    uint8_t m_address_type;
    uint8_t m_discovering;
  };

}

#endif //BABLE_LINUX_DISCOVERING_HPP
