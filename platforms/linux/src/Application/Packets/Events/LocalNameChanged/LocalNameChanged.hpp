#ifndef BABLE_LINUX_LOCALNAMECHANGED_HPP
#define BABLE_LINUX_LOCALNAMECHANGED_HPP

#include "../EventPacket.hpp"
#include "../../../../Exceptions/NotFound/NotFoundException.hpp"

namespace Packet {

  namespace Events {

    class LocalNameChanged : public EventPacket<LocalNameChanged> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::EventCode::LocalNameChanged;

          case Packet::Type::HCI:
            throw std::invalid_argument("'LocalNameChanged' packet is not compatible with HCI protocol.");

          case Packet::Type::ASCII:
            return Format::Ascii::EventCode::LocalNameChanged;

          case Packet::Type::FLATBUFFERS:
            throw Exceptions::NotFoundException("LocalNameChanged event has no event code for Flatbuffers.");

          case Packet::Type::NONE:
            return 0;
        }
      };

      LocalNameChanged(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(MGMTFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;

    private:
      std::array<uint8_t, 249> m_name{};
      std::array<uint8_t, 11> m_short_name{};
    };

  }

}

#endif //BABLE_LINUX_LOCALNAMECHANGED_HPP
