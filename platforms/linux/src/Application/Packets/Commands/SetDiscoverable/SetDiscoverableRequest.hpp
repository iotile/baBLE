#ifndef BABLE_LINUX_SETDISCOVERABLEREQUEST_HPP
#define BABLE_LINUX_SETDISCOVERABLEREQUEST_HPP

#include "../RequestPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetDiscoverableRequest : public RequestPacket<SetDiscoverableRequest> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::CommandCode::SetDiscoverable;

          case Packet::Type::HCI:
            throw std::invalid_argument("'SetDiscoverable' packet is not compatible with HCI protocol.");

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::SetDiscoverable);

          case Packet::Type::NONE:
            return 0;
        }
      };

      SetDiscoverableRequest(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      uint8_t m_state;
      uint16_t m_timeout;

    };

  }

}

#endif //BABLE_LINUX_SETDISCOVERABLEREQUEST_HPP
