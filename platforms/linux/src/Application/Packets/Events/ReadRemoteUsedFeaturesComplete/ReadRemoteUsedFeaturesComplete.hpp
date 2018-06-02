#ifndef BABLE_LINUX_READREMOTEUSEDFEATURESCOMPLETE_HPP
#define BABLE_LINUX_READREMOTEUSEDFEATURESCOMPLETE_HPP

#include "../EventPacket.hpp"
#include "../../../../Exceptions/NotFound/NotFoundException.hpp"

namespace Packet {

  namespace Events {

    class ReadRemoteUsedFeaturesComplete : public EventPacket<ReadRemoteUsedFeaturesComplete> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument(
                "'ReadRemoteUsedFeaturesComplete' packet is not compatible with MGMT protocol.");

          case Packet::Type::HCI:
            return Format::HCI::SubEventCode::LEReadRemoteUsedFeaturesComplete;

          case Packet::Type::ASCII:
            return Format::Ascii::EventCode::ReadRemoteUsedFeaturesComplete;

          case Packet::Type::FLATBUFFERS:
            throw Exceptions::NotFoundException(
                "ReadRemoteUsedFeaturesComplete event has no event code for Flatbuffers.");

          case Packet::Type::NONE:
            return 0;
        }
      };

      ReadRemoteUsedFeaturesComplete(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(HCIFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;

    private:
      bool m_le_encryption;
    };

  }

}

#endif //BABLE_LINUX_READREMOTEUSEDFEATURESCOMPLETE_HPP
