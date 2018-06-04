#include "DeviceRemoved.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    DeviceRemoved::DeviceRemoved(Packet::Type initial_type, Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_id = Packet::Id::DeviceRemoved;
      m_address_type = 0;
    }

    void DeviceRemoved::unserialize(MGMTFormatExtractor& extractor) {
      m_address = extractor.get_array<uint8_t, 6>();
      m_address_type = extractor.get_value<uint8_t>();
    };

    vector<uint8_t> DeviceRemoved::serialize(FlatbuffersFormatBuilder& builder) const {
      auto address = builder.CreateString(Utils::format_bd_address(m_address));

      auto payload = BaBLE::CreateDeviceRemoved(builder, address, m_address_type);

      return builder.build(payload, BaBLE::Payload::DeviceRemoved);
    }

    const std::string DeviceRemoved::stringify() const {
      stringstream result;

      result << "<DeviceRemoved> "
             << AbstractPacket::stringify() << ", "
             << "Address: " << Utils::format_bd_address(m_address) << ", "
             << "Address type: " << to_string(m_address_type);

      return result.str();
    }

  }

}