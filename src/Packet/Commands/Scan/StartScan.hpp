#ifndef BABLE_LINUX_COMMANDS_STARTSCAN_HPP
#define BABLE_LINUX_COMMANDS_STARTSCAN_HPP

#include <cstdint>
#include <flatbuffers/flatbuffers.h>
#include <Packet_generated.h>
#include "../CommandPacket.hpp"
#include "../../constants.hpp"

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

    StartScan(Packet::Type initial_type, Packet::Type translated_type): CommandPacket(initial_type, translated_type) {
      m_address_type = 0;
      m_params_length = 1;
    };

    void from_ascii(const std::vector<std::string>& params) override {
      if (params.size() < 2) {
        throw std::invalid_argument("Missing arguments for 'start_scan' packet. Usage: <command_code>,<controller_id>");
      }

      std::string controller_id_str = params.at(1);
      m_controller_id = static_cast<uint16_t>(stoi(controller_id_str));
    };

    void from_mgmt(Deserializer& deser) override {
      CommandPacket::from_mgmt(deser);
      deser >> m_address_type;
    };

    void from_flatbuffers(Deserializer& deser) override {
      auto packet = Schemas::GetPacket(deser.buffer());
      auto payload = static_cast<const Schemas::StartScan*>(packet->payload());

      flatbuffers::Verifier payload_verifier(deser.buffer(), deser.size());
      bool packet_valid = Schemas::VerifyPayload(payload_verifier, payload, Schemas::Payload::StartScan);
      if (!packet_valid) {
        throw std::invalid_argument("Flatbuffers StartScan packet is not valid. Can't parse it.");
      }

      m_controller_id = payload->controller_id();
      m_address_type = payload->address_type();
    };

    std::string to_ascii() const override {
      std::string header = CommandPacket::to_ascii();
      std::stringstream payload;

      payload << "Address type: " << std::to_string(m_address_type);

      return header + ", " + payload.str();
    };

    Serializer to_mgmt() const override {
      Serializer ser = CommandPacket::to_mgmt();
      ser << m_address_type;
      return ser;
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

#endif //BABLE_LINUX_COMMANDS_STARTSCAN_HPP
