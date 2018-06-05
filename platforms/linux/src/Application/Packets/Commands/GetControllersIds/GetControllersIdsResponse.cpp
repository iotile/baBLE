#include <sstream>
#include "GetControllersIdsResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetControllersIdsResponse::GetControllersIdsResponse()
        : ControllerToHostPacket(Packet::Id::GetControllersIdsResponse, initial_type(), initial_packet_code(), final_packet_code()) {}

    void GetControllersIdsResponse::unserialize(MGMTFormatExtractor& extractor) {
      set_status(extractor.get_value<uint8_t>());

      if (m_status == BaBLE::StatusCode::Success) {
        auto m_num_controllers = extractor.get_value<uint16_t>();
        m_controllers_ids = extractor.get_vector<uint16_t>(m_num_controllers);
      }
    }

    vector<uint8_t> GetControllersIdsResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto controllers_ids = builder.CreateVector(m_controllers_ids);
      auto payload = BaBLE::CreateGetControllersIds(builder, controllers_ids);

      return builder.build(payload, BaBLE::Payload::GetControllersIds);
    }

    const std::string GetControllersIdsResponse::stringify() const {
      stringstream result;

      result << "<GetControllersIdsResponse> "
             << AbstractPacket::stringify() << ", "
             << "Number of controllers: " << to_string(m_controllers_ids.size()) << ", "
             << "Controllers Id: [";

      for (auto it = m_controllers_ids.begin(); it != m_controllers_ids.end(); ++it) {
        result << to_string(*it);
        if (next(it) != m_controllers_ids.end()) {
          result << ", ";
        }
      }

      result << "]";

      return result.str();
    }

  }

}