#ifndef BABLE_LINUX_WRITEREQUEST_HPP
#define BABLE_LINUX_WRITEREQUEST_HPP

#include "../RequestPacket.hpp"

namespace Packet {

  namespace Commands {

    class WriteRequest : public RequestPacket<WriteRequest> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'Write' packet is not compatible with MGMT protocol.");

          case Packet::Type::HCI:
            return Format::HCI::AttributeCode::WriteRequest;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::Write);

          case Packet::Type::NONE:
            return 0;
        }
      };

      WriteRequest(Packet::Type initial_type, Packet::Type final_type);

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
      std::vector<uint8_t> m_data_to_write;

    };

  }

}

#endif //BABLE_LINUX_WRITEREQUEST_HPP
