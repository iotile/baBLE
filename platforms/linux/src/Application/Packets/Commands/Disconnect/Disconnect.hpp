#ifndef BABLE_LINUX_DISCONNECT_HPP
#define BABLE_LINUX_DISCONNECT_HPP

#include "../RequestPacket.hpp"

namespace Packet {

  namespace Commands {

    class Disconnect : public RequestPacket<Disconnect> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'Disconnect' packet not implemented with MGMT protocol.");

          case Packet::Type::HCI:
            return Format::HCI::CommandCode::Disconnect;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::Disconnect);

          case Packet::Type::NONE:
            return 0;
        }
      };

      Disconnect(Packet::Type initial_type, Packet::Type final_type);

      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void before_sent(const std::shared_ptr<PacketRouter>& router) override;

    private:
      uint8_t m_reason;

    };

  }

}

#endif //BABLE_LINUX_DISCONNECT_HPP
