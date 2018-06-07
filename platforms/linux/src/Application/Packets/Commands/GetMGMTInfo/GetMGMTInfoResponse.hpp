#ifndef BABLE_LINUX_GETMGMTINFORESPONSE_HPP
#define BABLE_LINUX_GETMGMTINFORESPONSE_HPP

#include "../ResponsePacket.hpp"

namespace Packet {

  namespace Commands {

    class GetMGMTInfoResponse : public ResponsePacket<GetMGMTInfoResponse> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::CommandCode::GetMGMTInfo;

          case Packet::Type::HCI:
            throw std::invalid_argument("'GetMGMTInfo' packet is not compatible with HCI protocol.");

          case Packet::Type::ASCII:
            return Format::Ascii::CommandCode::GetMGMTInfo;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::GetMGMTInfo);

          case Packet::Type::NONE:
            return 0;
        }
      };

      GetMGMTInfoResponse(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(MGMTFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

    private:
      uint8_t m_version;
      uint16_t m_revision;

    };

  }

}

#endif //BABLE_LINUX_GETMGMTINFORESPONSE_HPP
