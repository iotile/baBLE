#include "ReadByGroupTypeResponse.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Central {

      ReadByGroupTypeResponse::ReadByGroupTypeResponse()
        : ControllerToHostPacket(Packet::Id::ReadByGroupTypeResponse, initial_type(), initial_packet_code(), final_packet_code(), true) {
        m_last_group_end_handle = 0;
      }

      void ReadByGroupTypeResponse::unserialize(HCIFormatExtractor& extractor) {
        uint16_t data_length = extractor.get_data_length();

        if (data_length <= 0) {
          throw Exceptions::BaBLEException(
              BaBLE::StatusCode::WrongFormat,
              "Received HCI 'ReadByGroupTypeResponse' with no data."
          );
        }

        auto attribute_length = extractor.get_value<uint8_t>();
        data_length -= 1; // To remove attribute_length from remaining data_length

        size_t num_services = data_length / attribute_length;
        m_services.clear();
        m_services.reserve(num_services);

        while (data_length >= attribute_length) {
          Format::HCI::Service service{};
          service.handle = extractor.get_value<uint16_t>();
          service.group_end_handle = extractor.get_value<uint16_t>();
          service.uuid = extractor.get_vector<uint8_t>(attribute_length
                                                           - sizeof(service.handle)
                                                           - sizeof(service.group_end_handle));

          m_services.push_back(service);
          m_last_group_end_handle = service.group_end_handle;
          data_length -= attribute_length;
        }
      }

      const string ReadByGroupTypeResponse::stringify() const {
        stringstream result;

        result << "<ReadByGroupTypeResponse (Central)> "
               << AbstractPacket::stringify() << ", "
               << "Last group end handle: " << to_string(m_last_group_end_handle) << ", ";

        for (auto it = m_services.begin(); it != m_services.end(); ++it) {
          result << "{ Handle: " << to_string(it->handle) << ", "
                 << "Group end handle" << to_string(it->group_end_handle) << ", "
                 << "UUID" << Utils::format_uuid(it->uuid) << "}";
          if (next(it) != m_services.end()) {
            result << ", ";
          }
        }

        return result.str();
      }

    }

  }

}