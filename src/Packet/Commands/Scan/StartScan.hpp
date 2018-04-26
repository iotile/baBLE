#ifndef BABLE_LINUX_COMMANDS_STARTSCAN_HPP
#define BABLE_LINUX_COMMANDS_STARTSCAN_HPP

#include <cstdint>
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

        default:
          throw std::runtime_error("Current type has no known id.");
      }
    };

    StartScan(Packet::Type initial_type, Packet::Type translated_type): CommandPacket(initial_type, translated_type) {
      m_address_type = 0x06;
      m_params_length = 1;
    };

    void from_ascii(const std::vector<std::string>& params) override {
      if (params.size() < 2) {
        throw std::invalid_argument("Missing arguments for 'start_scan' packet. Usage: <command_code>,<controller_id>");
      }

      std::string controller_id_str = params.at(1);
      m_controller_id = static_cast<uint16_t>(stoi(controller_id_str));
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

  private:
    uint8_t m_address_type;

  };

}

#endif //BABLE_LINUX_COMMANDS_STARTSCAN_HPP
