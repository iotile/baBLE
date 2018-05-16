#ifndef BABLE_LINUX_NEWSETTINGS_HPP
#define BABLE_LINUX_NEWSETTINGS_HPP

#include "../EventPacket.hpp"
#include "../../../../Exceptions/NotFound/NotFoundException.hpp"

namespace Packet::Events {

  class NewSettings : public EventPacket<NewSettings> {

  public:
    static const uint16_t event_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::EventCode::NewSettings;

        case Packet::Type::HCI:
          throw std::invalid_argument("'NewSettings' packet is not compatible with HCI protocol.");

        case Packet::Type::ASCII:
          return Format::Ascii::EventCode::NewSettings;

        case Packet::Type::FLATBUFFERS:
          throw Exceptions::NotFoundException("NewSettings event has no event code for Flatbuffers.");

        case Packet::Type::NONE:
          return 0;
      }
    };

    NewSettings(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;

  private:
    uint32_t m_current_settings;
  };

}

#endif //BABLE_LINUX_NEWSETTINGS_HPP
