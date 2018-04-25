#ifndef BABLE_LINUX_RESPONSES_GETMGMTINFO_HPP
#define BABLE_LINUX_RESPONSES_GETMGMTINFO_HPP

#include <cstdint>
#include "../ResponsePacket.hpp"
#include "../../constants.hpp"

namespace Packet::Responses {

  class GetMGMTInfo : public ResponsePacket {

  public:
    static const uint16_t command_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Commands::MGMT::Code::GetMGMTInfo;

        case Packet::Type::ASCII:
          return Commands::Ascii::Code::GetMGMTInfo;

        default:
          throw std::runtime_error("Current type has no known id.");
      }
    };

    GetMGMTInfo(Packet::Type initial_type, Packet::Type translated_type): ResponsePacket(initial_type, translated_type) {
      m_version = 0;
      m_revision = 0;
      m_command_code = command_code(m_current_type);
    };

    void from_mgmt(Deserializer& deser) override {
      ResponsePacket::from_mgmt(deser);
      deser >> m_version >> m_revision;
    };

    std::string to_ascii() const override {
      std::stringstream result;
      generate_header(result);

      result << "Version: " << std::to_string(m_version) << ", "
             << "Revision: " << std::to_string(m_revision);

      return result.str();
    };

  private:
    uint8_t m_version;
    uint16_t m_revision;
  };

}

#endif //BABLE_LINUX_RESPONSES_GETMGMTINFO_HPP
