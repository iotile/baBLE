#ifndef BABLE_LINUX_GETMGMTINFORESPONSE_HPP
#define BABLE_LINUX_GETMGMTINFORESPONSE_HPP

#include "../../Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Commands {

    class GetMGMTInfoResponse : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::MGMT;
      };

      static const uint16_t initial_packet_code() {
        return Format::MGMT::CommandCode::GetMGMTInfo;
      };

      static const uint16_t final_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::GetMGMTInfo);
      };

      GetMGMTInfoResponse();

      void unserialize(MGMTFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      uint8_t m_version;
      uint16_t m_revision;

    };

  }

}

#endif //BABLE_LINUX_GETMGMTINFORESPONSE_HPP
