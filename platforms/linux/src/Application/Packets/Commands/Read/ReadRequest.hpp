#ifndef BABLE_LINUX_READREQUEST_HPP
#define BABLE_LINUX_READREQUEST_HPP

#include "../RequestPacket.hpp"

namespace Packet {

  namespace Commands {

    class ReadRequest : public RequestPacket<ReadRequest> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'Read' packet is not compatible with MGMT protocol.");

          case Packet::Type::HCI:
            return Format::HCI::AttributeCode::ReadRequest;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::Read);

          case Packet::Type::NONE:
            return 0;
        }
      };

      ReadRequest(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void before_sent(const std::shared_ptr<PacketRouter>& router) override;
      std::shared_ptr<Packet::AbstractPacket> on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                   const std::shared_ptr<AbstractPacket>& packet) override;
      std::shared_ptr<Packet::AbstractPacket> on_error_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                         const std::shared_ptr<AbstractPacket>& packet);

    private:
      uint16_t m_attribute_handle;
    };

  }

}

#endif //BABLE_LINUX_READREQUEST_HPP
