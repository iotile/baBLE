#ifndef BABLE_LINUX_FLATBUFFERSBUILDER_HPP
#define BABLE_LINUX_FLATBUFFERSBUILDER_HPP

#include <flatbuffers/flatbuffers.h>
#include <Packet_generated.h>
#include "../AbstractBuilder.hpp"
#include "../../Log/Log.hpp"

class FlatbuffersBuilder : public AbstractBuilder {

public:
  std::unique_ptr<Packet::AbstractPacket> build(Deserializer& deser) override {
    deser.reverse();

    flatbuffers::Verifier packet_verifier(deser.buffer(), deser.size());
    bool packet_valid = Schemas::VerifyPacketBuffer(packet_verifier);

    if (!packet_valid) {
      throw std::invalid_argument("Flatbuffers packet is not valid. Can't build packet from it.");
    }

    auto fb_packet = Schemas::GetPacket(deser.buffer());
    auto payload_type = fb_packet->payload_type();

    auto it = m_commands.find(static_cast<uint16_t>(payload_type));

    if (it == m_commands.end()) {
      throw std::invalid_argument("Packet type not found");
    }

    PacketConstructor fn = it->second;
    std::unique_ptr<Packet::AbstractPacket> packet = fn();
    packet->unserialize(deser);
    return packet;
  };

  template<class T>
  AbstractBuilder& register_event(Packet::Type translated_type) = delete;

private:
  const Packet::Type packet_type() const override {
    return Packet::Type::FLATBUFFERS;
  };
};

#endif //BABLE_LINUX_FLATBUFFERSBUILDER_HPP
