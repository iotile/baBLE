#include "ReadResponse.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    ReadResponse::ReadResponse(Packet::Type initial_type, Packet::Type final_type)
        : ResponsePacket(initial_type, final_type) {
      m_id = Packet::Id::ReadResponse;
      m_attribute_handle = 0;
    }

    void ReadResponse::unserialize(HCIFormatExtractor& extractor) {
      uint16_t data_length = extractor.get_data_length();
      m_data_read = extractor.get_vector<uint8_t>(data_length);
    }

    vector<uint8_t> ReadResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto data_read = builder.CreateVector(m_data_read);
      auto payload = BaBLE::CreateRead(
          builder,
          m_connection_id,
          m_attribute_handle,
          data_read
      );

      return builder.build(payload, BaBLE::Payload::Read);
    }

    const std::string ReadResponse::stringify() const {
      stringstream result;

      result << "<ReadResponse> "
             << AbstractPacket::stringify() << ", "
             << "Attribute handle: " << to_string(m_attribute_handle) << ", "
             << "Data: " << Utils::format_bytes_array(m_data_read);

      return result.str();
    }

  }

}