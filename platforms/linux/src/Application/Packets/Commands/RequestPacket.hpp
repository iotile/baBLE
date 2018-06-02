#ifndef BABLE_LINUX_REQUESTPACKET_HPP
#define BABLE_LINUX_REQUESTPACKET_HPP

#include "../../PacketRouter/PacketRouter.hpp"
#include "../../../Log/Log.hpp"

namespace Packet {

  namespace Commands {

    template<class T>
    class RequestPacket : public AbstractPacket {

    protected:
      RequestPacket(Packet::Type initial_type, Packet::Type translated_type)
          : AbstractPacket(initial_type, translated_type) {
        m_packet_code = T::packet_code(m_current_type);
        m_response_packet_code = T::packet_code(m_translated_type);
      };

      PacketUuid get_response_uuid() const {
        PacketUuid response_uuid = get_uuid();
        response_uuid.response_packet_code = m_response_packet_code;

        return response_uuid;
      }

      void before_sent(const std::shared_ptr<PacketRouter>& router) override {
        AbstractPacket::before_sent(router);
        m_packet_code = T::packet_code(m_current_type);

        if (m_current_type == m_translated_type) {
          PacketUuid response_uuid = get_response_uuid();
          auto response_callback =
              [this](const std::shared_ptr<PacketRouter>& router, const std::shared_ptr<AbstractPacket>& packet) {
                return on_response_received(router, packet);
              };
          router->add_callback(response_uuid, shared_from(this), response_callback);
        }
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
        builder.set_opcode(m_packet_code);

        return {};
      };

      virtual std::shared_ptr<Packet::AbstractPacket> on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                           const std::shared_ptr<AbstractPacket>& packet) {
        LOG.debug("Response received", "RequestPacket");
        packet->set_uuid_request(m_uuid_request);
        return packet;
      };

      uint16_t m_response_packet_code;

    };

  }

}

#endif //BABLE_LINUX_REQUESTPACKET_HPP