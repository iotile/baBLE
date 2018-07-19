#include <utils/string_formats.hpp>
#include "SetScanResponse.hpp"
#include "Application/Packets/Events/CommandComplete/CommandComplete.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetScanResponse::SetScanResponse()
        : HostToControllerPacket(Packet::Id::SetScanResponse, final_type(), final_packet_code()) {
      m_response_packet_code = Format::HCI::EventCode::CommandComplete;

      m_scan_response_length = 0;
      m_scan_response = vector<uint8_t>(Format::HCI::advertising_data_length);
    }

    vector<uint8_t> SetScanResponse::serialize(HCIFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);

      builder
          .add(m_scan_response_length)
          .add(m_scan_response);

      return builder.build(Format::HCI::Type::Command);
    }

    void SetScanResponse::set_scan_response_data(const vector<uint8_t>& scan_response) {
      if (scan_response.size() > Format::HCI::advertising_data_length) {
        throw Exceptions::BaBLEException(BaBLE::StatusCode::WrongFormat, "Scan response exceeds 31 bytes", m_uuid_request);
      }

      m_scan_response_length = static_cast<uint8_t>(scan_response.size());
      copy(scan_response.begin(),scan_response.end(), m_scan_response.begin());
    }

    const string SetScanResponse::stringify() const {
      stringstream result;

      result << "<SetScanResponse> "
             << AbstractPacket::stringify() << ", "
             << "Scan response data: " << Utils::format_bytes_array(m_scan_response);

      return result.str();
    }

  }

}
