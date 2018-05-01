#ifndef BABLE_LINUX_EVENTPACKET_HPP
#define BABLE_LINUX_EVENTPACKET_HPP

#include "../AbstractPacket.hpp"

namespace Packet::Events {

  template<class T>
  class EventPacket : public AbstractPacket {

  protected:
    EventPacket(Packet::Type initial_type, Packet::Type translated_type): AbstractPacket(initial_type, translated_type) {
      m_controller_id = 0xFFFF;
    };

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override {
      builder
          .add("Type", "Event");
    }

    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override {};
    std::vector<uint8_t> serialize(flatbuffers::FlatBufferBuilder& builder) const override {};

    void import(MGMTFormatExtractor& extractor) override {};
    void import(AsciiFormatExtractor& extractor) override {};
    void import(const Schemas::Packet* packet) override {};

  };

}

#endif //BABLE_LINUX_EVENTPACKET_HPP