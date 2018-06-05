#ifndef BABLE_LINUX_WRITEWITHOUTRESPONSE_HPP
#define BABLE_LINUX_WRITEWITHOUTRESPONSE_HPP

#include "../../Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class WriteWithoutResponse : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::WriteWithoutResponse);
      };

      static const uint16_t final_packet_code() {
        return Format::HCI::AttributeCode::WriteCommand;
      };

      explicit WriteWithoutResponse(uint16_t attribute_handle = 0, std::vector<uint8_t> data = {});

      void unserialize(FlatbuffersFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      uint16_t m_attribute_handle;
      std::vector<uint8_t> m_data_to_write;

    };

  }

}

#endif //BABLE_LINUX_WRITEWITHOUTRESPONSE_HPP
