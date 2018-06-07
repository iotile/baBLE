#ifndef BABLE_LINUX_WRITEREQUEST_HPP
#define BABLE_LINUX_WRITEREQUEST_HPP

#include "../../Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class WriteRequest : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::Write);
      };

      static const uint16_t final_packet_code() {
        return Format::HCI::AttributeCode::WriteRequest;
      };

      explicit WriteRequest(uint16_t attribute_handle = 0, std::vector<uint8_t> data = {});

      void unserialize(FlatbuffersFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void prepare(const std::shared_ptr<PacketRouter>& router) override;
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
