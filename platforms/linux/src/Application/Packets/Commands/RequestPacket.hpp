#ifndef BABLE_LINUX_REQUESTPACKET_HPP
#define BABLE_LINUX_REQUESTPACKET_HPP

#include <functional>
#include "../../AbstractPacket.hpp"
#include "../../PacketRouter/PacketRouter.hpp"

namespace Packet::Commands {

  template<class T>
  class RequestPacket : public AbstractPacket {

  protected:
    RequestPacket(Packet::Type initial_type, Packet::Type translated_type)
        : AbstractPacket(initial_type, translated_type) {
      m_packet_code = T::packet_code(m_current_type);
    };

    void before_sent(const std::shared_ptr<PacketRouter>& router) override {
      AbstractPacket::before_sent(router);
      m_packet_code = T::packet_code(m_current_type);

      auto callback = [this](std::shared_ptr<Packet::AbstractPacket> packet) {
        return this->on_response_received(packet);
      };

      router->add_callback(get_uuid(), callback);
    };

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override {
      builder.add("Type", "Request");

      return {};
    };

    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override {
      builder.set_opcode(m_packet_code);

      return {};
    };

    std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override {
      builder.set_opcode(static_cast<uint8_t>(m_packet_code));

      return {};
    };

    std::shared_ptr<Packet::AbstractPacket> on_response_received(std::shared_ptr<Packet::AbstractPacket> packet) {
      LOG.debug("Response received", "RequestPacket");
      packet->set_uuid_request(m_uuid_request);
      return move(packet);
    };

  };

}

#endif //BABLE_LINUX_REQUESTPACKET_HPP