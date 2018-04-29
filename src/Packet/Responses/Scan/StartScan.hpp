#ifndef BABLE_LINUX_RESPONSES_STARTSCAN_HPP
#define BABLE_LINUX_RESPONSES_STARTSCAN_HPP

#include <cstdint>
#include <flatbuffers/flatbuffers.h>
#include <Packet_generated.h>
#include "../ResponsePacket.hpp"
#include "../../constants.hpp"

namespace Packet::Responses {

  class StartScan : public ResponsePacket<StartScan> {

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

    StartScan(Packet::Type initial_type, Packet::Type translated_type): ResponsePacket(initial_type, translated_type) {
      m_address_type = 0;
    };

    void from_mgmt(Deserializer& deser) override {
      ResponsePacket::from_mgmt(deser);
      deser >> m_address_type;
    };

    std::string to_ascii() const override {
      std::string header = ResponsePacket::to_ascii();
      std::stringstream payload;

      payload << "Address type: " << std::to_string(m_address_type);


      return header + ", " + payload.str();
    };

    Serializer to_flatbuffers() const override {
      flatbuffers::FlatBufferBuilder builder(0);
      auto payload = Schemas::CreateStartScan(builder, m_controller_id, m_address_type);

      Serializer ser = build_flatbuffers_packet<Schemas::StartScan>(builder, payload, Schemas::Payload::StartScan);

      Serializer result;
      result << static_cast<uint8_t>(0xCA) << static_cast<uint8_t>(0xFE) << static_cast<uint16_t>(ser.size()) << ser;

      return result;
    }

  private:
    uint8_t m_address_type;
  };

}

#endif //BABLE_LINUX_RESPONSES_STARTSCAN_HPP
