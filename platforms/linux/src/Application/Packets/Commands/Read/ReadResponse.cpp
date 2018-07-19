#include "ReadResponse.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    ReadResponse::ReadResponse()
        : ControllerToHostPacket(Packet::Id::ReadResponse, initial_type(), initial_packet_code(), final_packet_code()) {
      m_attribute_handle = 0;
    }

    void ReadResponse::unserialize(HCIFormatExtractor& extractor) {
      uint16_t data_length = extractor.get_data_length();
      m_data_read = extractor.get_vector<uint8_t>(data_length);
    }

    vector<uint8_t> ReadResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto data_read = builder.CreateVector(m_data_read);
      auto payload = BaBLE::CreateReadCentral(
          builder,
          m_connection_handle,
          m_attribute_handle,
          data_read
      );

      return builder.build(payload, BaBLE::Payload::ReadCentral);
    }

    const string ReadResponse::stringify() const {
      stringstream result;

      result << "<ReadResponse> "
             << AbstractPacket::stringify() << ", "
             << "Attribute handle: " << to_string(m_attribute_handle) << ", "
             << "Data: " << Utils::format_bytes_array(m_data_read);

      return result.str();
    }

  }

}