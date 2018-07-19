#include <utils/string_formats.hpp>
#include "SetAdvertisingData.hpp"
#include "Application/Packets/Events/CommandComplete/CommandComplete.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetAdvertisingData::SetAdvertisingData()
        : HostToControllerPacket(Packet::Id::SetAdvertisingData, final_type(), final_packet_code()) {
      m_response_packet_code = Format::HCI::EventCode::CommandComplete;

      m_advertising_data_length = 0;
      m_advertising_data = vector<uint8_t>(Format::HCI::advertising_data_length);
    }

    vector<uint8_t> SetAdvertisingData::serialize(HCIFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);

      builder
          .add(m_advertising_data_length)
          .add(m_advertising_data);

      return builder.build(Format::HCI::Type::Command);
    }

    void SetAdvertisingData::set_advertising_data(const vector<uint8_t>& advertising_data) {
      if (advertising_data.size() > Format::HCI::advertising_data_length) {
        throw Exceptions::BaBLEException(BaBLE::StatusCode::WrongFormat, "Advertisement data exceeds 31 bytes", m_uuid_request);
      }

      m_advertising_data_length = static_cast<uint8_t>(advertising_data.size());
      copy(advertising_data.begin(),advertising_data.end(), m_advertising_data.begin());
    }

    const string SetAdvertisingData::stringify() const {
      stringstream result;

      result << "<SetAdvertisingData> "
             << AbstractPacket::stringify() << ", "
             << "Advertising data: " << Utils::format_bytes_array(m_advertising_data);

      return result.str();
    }

  }

}
