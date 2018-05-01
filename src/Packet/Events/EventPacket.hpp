#ifndef BABLE_LINUX_EVENTPACKET_HPP
#define BABLE_LINUX_EVENTPACKET_HPP

#include "../AbstractPacket.hpp"

namespace Packet::Events {

  template<class T>
  class EventPacket : public AbstractPacket {

  protected:
    EventPacket(Packet::Type initial_type, Packet::Type translated_type): AbstractPacket(initial_type, translated_type) {
      m_controller_id = Format::MGMT::non_controller_id;
    };

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override {
      builder
          .add("Type", "Event");

      return {};
    }

    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override {
      return {};
    };

    void import(MGMTFormatExtractor& extractor) override {};

  };

}

#endif //BABLE_LINUX_EVENTPACKET_HPP