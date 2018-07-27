#ifndef BABLE_SETSERVICES_HPP
#define BABLE_SETSERVICES_HPP

#include "Application/Packets/Base/HostOnlyPacket.hpp"

namespace Packet {

  namespace Control {

    class SetGATTTable : public HostOnlyPacket {

    public:
      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::SetGATTTable);
      };

      SetGATTTable();

      void unserialize(FlatbuffersFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void set_socket(AbstractSocket* socket) override;

    private:
      std::vector<Format::HCI::Service> m_services;
      std::vector<Format::HCI::Characteristic> m_characteristics;

    };

  }

}

#endif //BABLE_SETSERVICES_HPP
