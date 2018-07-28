#ifndef BABLE_NUMBEROFCOMPLETEDPACKETS_HPP
#define BABLE_NUMBEROFCOMPLETEDPACKETS_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Events {

    class NumberOfCompletedPackets : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return Format::HCI::EventCode::NumberOfCompletedPackets;
      };

      static const uint16_t final_packet_code() {
        return initial_packet_code();
      };

      NumberOfCompletedPackets();

      void unserialize(HCIFormatExtractor& extractor) override;

      void set_socket(AbstractSocket* socket) override;

      const std::string stringify() const override;

    private:
      std::vector<std::tuple<uint16_t, uint16_t>> m_completed_packets;

    };

  }

}


#endif //BABLE_NUMBEROFCOMPLETEDPACKETS_HPP
