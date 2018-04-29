#ifndef BABLE_LINUX_RESPONSES_STOPSCAN_HPP
#define BABLE_LINUX_RESPONSES_STOPSCAN_HPP

#include <cstdint>
#include <flatbuffers/flatbuffers.h>
#include <Packet_generated.h>
#include "../ResponsePacket.hpp"
#include "../../constants.hpp"

namespace Packet::Responses {

  class StopScan : public ResponsePacket<StopScan> {

  public:
    static const uint16_t command_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Commands::MGMT::Code::StopScan;

        case Packet::Type::ASCII:
          return Commands::Ascii::Code::StopScan;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::StopScan);

        default:
          throw std::runtime_error("Current type has no known id (StopScan).");
      }
    };

    StopScan(Packet::Type initial_type, Packet::Type translated_type): ResponsePacket(initial_type, translated_type) {
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
      auto payload = Schemas::CreateStopScan(builder, m_controller_id, m_address_type);

      Serializer ser = build_flatbuffers_packet<Schemas::StopScan>(builder, payload, Schemas::Payload::StopScan);

      Serializer result;
      result << static_cast<uint8_t>(0xCA) << static_cast<uint8_t>(0xFE) << static_cast<uint16_t>(ser.size()) << ser;

      return result;
    }

  private:
    uint8_t m_address_type;
  };

}

#endif //BABLE_LINUX_RESPONSES_STOPSCAN_HPP
