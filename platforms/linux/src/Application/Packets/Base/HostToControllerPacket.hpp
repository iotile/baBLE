#ifndef BABLE_HOSTTOCONTROLLERPACKET_HPP
#define BABLE_HOSTTOCONTROLLERPACKET_HPP

#include "Application/PacketRouter/PacketRouter.hpp"
#include "Log/Log.hpp"

namespace Packet {

  class HostToControllerPacket : public AbstractPacket {

  public:
    static Packet::Type initial_type() {
      return Packet::Type::FLATBUFFERS;
    };

    PacketUuid get_response_uuid() const;

  protected:
    HostToControllerPacket(Packet::Id id, Packet::Type type, uint16_t packet_code, bool waiting_response = true);

    void set_waiting_response(bool waiting_response);

    void prepare(const std::shared_ptr<PacketRouter>& router) override;

    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

    virtual std::shared_ptr<AbstractPacket> on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                         const std::shared_ptr<AbstractPacket>& packet);

    uint16_t m_response_packet_code;
    bool m_waiting_response;

  };

}

#endif //BABLE_HOSTTOCONTROLLERPACKET_HPP