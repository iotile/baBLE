#ifndef BABLE_LINUX_RESPONSES_STARTSCAN_HPP
#define BABLE_LINUX_RESPONSES_STARTSCAN_HPP

#include <cstdint>
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

        default:
          throw std::runtime_error("Current type has no known id.");
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

  private:
    uint8_t m_address_type;
  };

}

#endif //BABLE_LINUX_RESPONSES_STARTSCAN_HPP
