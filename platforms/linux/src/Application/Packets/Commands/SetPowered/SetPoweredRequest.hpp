#ifndef BABLE_LINUX_SETPOWEREDREQUEST_HPP
#define BABLE_LINUX_SETPOWEREDREQUEST_HPP

#include "../RequestPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetPoweredRequest : public RequestPacket<SetPoweredRequest> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::CommandCode::SetPowered;

          case Packet::Type::HCI:
            throw std::invalid_argument("'SetPowered' packet is not compatible with HCI protocol.");

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::SetPowered);

          case Packet::Type::NONE:
            return 0;
        }
      };

      SetPoweredRequest(Packet::Type initial_type, Packet::Type final_type);

      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      bool m_state;

    };

  }

}

#endif //BABLE_LINUX_SETPOWEREDREQUEST_HPP
