#include "ReadByTypeResponse.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    ReadByTypeResponse::ReadByTypeResponse()
        : ControllerToHostPacket(Packet::Id::ReadByTypeResponse, initial_type(), initial_packet_code(), final_packet_code(), true) {
      m_last_ending_handle = 0;
    }

    void ReadByTypeResponse::unserialize(HCIFormatExtractor& extractor) {
      uint16_t data_length = extractor.get_data_length();

      if (data_length <= 0) {
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::WrongFormat,
            "Received HCI 'ReadByTypeResponse' with no data."
        );
      }

      auto attribute_length = extractor.get_value<uint8_t>();
      data_length -= 1; // To remove attribute_length from remaining data_length

      size_t num_characteristics = data_length / attribute_length;
      m_characteristics.clear();
      m_characteristics.reserve(num_characteristics);

      while (data_length >= attribute_length) {
        Format::HCI::Characteristic characteristic{};

        if (attribute_length == 7) {
          // Contains a GATT Characteristic Declaration
          characteristic.handle = extractor.get_value<uint16_t>();
          characteristic.properties = extractor.get_value<uint8_t>();
          characteristic.value_handle = extractor.get_value<uint16_t>();
          characteristic.uuid = extractor.get_vector<uint8_t>(attribute_length
                                                                  - sizeof(characteristic.handle)
                                                                  - sizeof(characteristic.properties)
                                                                  - sizeof(characteristic.value_handle));
          m_last_ending_handle = characteristic.value_handle;
        } else {
          // Contains a GATT Client Characteristic Configuration
          characteristic.config_handle = extractor.get_value<uint16_t>();
          characteristic.configuration = extractor.get_value<uint16_t>();
          m_last_ending_handle = characteristic.config_handle;
        }

        m_characteristics.push_back(characteristic);
        data_length -= attribute_length;
      }
    }

    const string ReadByTypeResponse::stringify() const {
      stringstream result;

      result << "<ReadByTypeResponse> "
             << AbstractPacket::stringify() << ", "
             << "Last ending handle: " << to_string(m_last_ending_handle) << ", ";

      for (auto it = m_characteristics.begin(); it != m_characteristics.end(); ++it) {
        result << "{ Handle: " << to_string(it->handle) << ", "
               << "Properties" << to_string(it->properties) << ", "
               << "Value handle" << to_string(it->value_handle) << ", "
               << "Config handle" << to_string(it->config_handle) << ", "
               << "Configuration" << to_string(it->configuration) << ", "
               << "UUID" << Utils::format_uuid(it->uuid) << "}";
        if (next(it) != m_characteristics.end()) {
          result << ", ";
        }
      }

      return result.str();
    }

  }

}