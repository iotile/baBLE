#ifndef BABLE_LINUX_CLASSOFDEVICECHANGED_HPP
#define BABLE_LINUX_CLASSOFDEVICECHANGED_HPP

#include "../EventPacket.hpp"
#include "../../../../Exceptions/NotFound/NotFoundException.hpp"

namespace Packet::Events {

  class ClassOfDeviceChanged : public EventPacket<ClassOfDeviceChanged> {

  public:
    static const uint16_t event_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::EventCode::ClassOfDeviceChanged;

        case Packet::Type::HCI:
          throw std::invalid_argument("'ClassOfDeviceChanged' packet is not compatible with HCI protocol.");

        case Packet::Type::ASCII:
          return Format::Ascii::EventCode::ClassOfDeviceChanged;

        case Packet::Type::FLATBUFFERS:
          throw Exceptions::NotFoundException("ClassOfDeviceChanged event has no event code for Flatbuffers.");

        case Packet::Type::NONE:
          return 0;
      }
    };

    ClassOfDeviceChanged(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;

  private:
    std::array<uint8_t, 3> m_class_of_device{};
  };

}

#endif //BABLE_LINUX_CLASSOFDEVICECHANGED_HPP
