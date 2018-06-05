#ifndef BABLE_LINUX_HOSTTOCONTROLLERPACKET_HPP
#define BABLE_LINUX_HOSTTOCONTROLLERPACKET_HPP

#include "../../PacketRouter/PacketRouter.hpp"
#include "../../../Log/Log.hpp"

namespace Packet {

  class HostToControllerPacket : public AbstractPacket {

  public:
    static Packet::Type initial_type() {
      return Packet::Type::FLATBUFFERS;
    };

    PacketUuid get_response_uuid() const {
      PacketUuid response_uuid = get_uuid();
      response_uuid.response_packet_code = m_response_packet_code;

      return response_uuid;
    };

  protected:
    HostToControllerPacket(Packet::Id id, Packet::Type type, uint16_t packet_code, bool waiting_response = true)
        : AbstractPacket(id, initial_type(), type, packet_code) {
      m_response_packet_code = m_packet_code;
      m_waiting_response = waiting_response;
    };

    void set_waiting_response(bool waiting_response) { // TODO: is this useful ?
      m_waiting_response = waiting_response;
    };

    void prepare(const std::shared_ptr<PacketRouter>& router) override {
      AbstractPacket::translate();

      if (m_waiting_response) {
        PacketUuid response_uuid = get_response_uuid();
        auto response_callback =
            [this](const std::shared_ptr<PacketRouter>& router, const std::shared_ptr<AbstractPacket>& packet) {
              return on_response_received(router, packet);
            };
        router->add_callback(response_uuid, shared_from(this), response_callback);
      }
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
    bool m_waiting_response;

  };

}

#endif //BABLE_LINUX_HOSTTOCONTROLLERPACKET_HPP