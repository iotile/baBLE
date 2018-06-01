#ifndef BABLE_LINUX_GETCONTROLLERSIDSRESPONSE_HPP
#define BABLE_LINUX_GETCONTROLLERSIDSRESPONSE_HPP

#include "../ResponsePacket.hpp"

namespace Packet {

  namespace Commands {

    class GetControllersIdsResponse : public ResponsePacket<GetControllersIdsResponse> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::CommandCode::GetControllersList;

          case Packet::Type::HCI:
            throw std::invalid_argument("'GetControllersIds' packet is not compatible with HCI protocol.");

          case Packet::Type::ASCII:
            return Format::Ascii::CommandCode::GetControllersIds;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::GetControllersIds);

          case Packet::Type::NONE:
            return 0;
        }
      };

      GetControllersIdsResponse(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(MGMTFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      inline const std::vector<uint16_t>& get_controllers_ids() const {
        return m_controllers_ids;
      }

    private:
      std::vector<uint16_t> m_controllers_ids;

    };

  }

}

#endif //BABLE_LINUX_GETCONTROLLERSIDSRESPONSE_HPP
