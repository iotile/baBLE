#include "GetControllersIdsResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetControllersIdsResponse::GetControllersIdsResponse(Packet::Type initial_type, Packet::Type translated_type)
        : ResponsePacket(initial_type, translated_type) {
      m_id = Packet::Id::GetControllersIdsResponse;
    }

    void GetControllersIdsResponse::unserialize(MGMTFormatExtractor& extractor) {
      ResponsePacket::unserialize(extractor);

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