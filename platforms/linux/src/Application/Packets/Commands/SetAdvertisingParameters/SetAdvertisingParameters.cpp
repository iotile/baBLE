#include "SetAdvertisingParameters.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetAdvertisingParameters::SetAdvertisingParameters(uint16_t interval_min, uint16_t interval_max, uint8_t advertising_type,
                                                       uint8_t own_address_type, uint8_t direct_address_type,
                                                       uint8_t channels, uint8_t policy)
        : HostToControllerPacket(Packet::Id::SetAdvertisingParameters, final_type(), final_packet_code()) {
      m_response_packet_code = Format::HCI::EventCode::CommandComplete;

      m_interval_min = interval_min;
      m_interval_max = interval_max;
      m_advertising_type = advertising_type;
      m_own_address_type = own_address_type;
      m_direct_address_type = direct_address_type;
      m_channels = channels;
      m_policy = policy;
    }

    vector<uint8_t> SetAdvertisingParameters::serialize(HCIFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);

      builder
          .add(m_interval_min)
          .add(m_interval_max)
          .add(m_advertising_type)
          .add(m_own_address_type)
          .add(m_direct_address_type)
          .add(m_address)
          .add(m_channels)
          .add(m_policy);

      return builder.build(Format::HCI::Type::Command);
    }

    const string SetAdvertisingParameters::stringify() const {
      stringstream result;

      result << "<SetAdvertisingParameters> "
             << AbstractPacket::stringify() << ", "
             << "Interval min: " << to_string(m_interval_min) << ", "
             << "Interval max: " << to_string(m_interval_max) << ", "
             << "Advertising type: " << to_string(m_advertising_type) << ", "
             << "Own address type: " << to_string(m_own_address_type) << ", "
             << "Direct address type: " << to_string(m_direct_address_type) << ", "
             << "Address" << Utils::format_bd_address(m_address) << ", "
             << "Channels: " << to_string(m_channels) << ", "
             << "Policy: " << to_string(m_policy);

      return result.str();
    }

  }

}
