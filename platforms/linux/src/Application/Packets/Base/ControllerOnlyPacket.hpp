#ifndef BABLE_CONTROLLERONLYPACKET_HPP
#define BABLE_CONTROLLERONLYPACKET_HPP

#include "Application/Packets/AbstractPacket.hpp"

namespace Packet {

  class ControllerOnlyPacket : public AbstractPacket {

  protected:
    ControllerOnlyPacket(Packet::Id id, Packet::Type type, uint16_t initial_packet_code, uint16_t final_packet_code)
        : AbstractPacket(id, type, type, initial_packet_code) {
      switch (type) {
        case Packet::Type::HCI:
          m_native_class = "HCI";
          break;

        case Packet::Type::MGMT:
          m_native_class = "MGMT";
          break;

        default:
          m_native_class = "BaBLE";
          break;
      }

      m_final_packet_code = final_packet_code;
    };

    void prepare(const std::shared_ptr<PacketRouter>& router) override {
      m_packet_code = m_final_packet_code;
    };

    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override {
      builder.set_opcode(m_packet_code);

      return {};
    };

    std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override {
      builder.set_opcode(m_packet_code);

      return {};
    };


  private:
    uint16_t m_final_packet_code;

  };

}

#endif //BABLE_CONTROLLERONLYPACKET_HPP
