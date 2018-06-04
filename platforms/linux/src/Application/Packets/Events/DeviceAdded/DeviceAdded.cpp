#include "DeviceAdded.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    DeviceAdded::DeviceAdded(Packet::Type initial_type, Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_id = Packet::Id::DeviceAdded;
      m_address_type = 0;
      m_action = 0;
    }

    void DeviceAdded::unserialize(MGMTFormatExtractor& extractor) {
      m_address = extractor.get_array<uint8_t, 6>();
      m_address_type = extractor.get_value<uint8_t>();
      m_action = extractor.get_value<uint8_t>();
    };

    vector<uint8_t> DeviceAdded::serialize(FlatbuffersFormatBuilder& builder) const {
      auto address = builder.CreateString(Utils::format_bd_address(m_address));

      auto payload = BaBLE::CreateDeviceAdded(builder, address, m_address_type);

      return builder.build(payload, BaBLE::Payload::DeviceAdded);
    }

    const std::string DeviceAdded::stringify() const {
      stringstream result;

      result << "<DeviceAdded> "
             << AbstractPacket::stringify() << ", "
             << "Address: " << Utils::format_bd_address(m_address) << ", "
             << "Address type: " << to_string(m_address_type) << ", "
             << "Action: " << to_string(m_action);

      return result.str();
    }

  }

}