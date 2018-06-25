#include <sstream>
#include "BaBLEError.hpp"

using namespace std;

namespace Packet {

  namespace Errors {

    BaBLEError::BaBLEError(const Exceptions::BaBLEException& exception)
        : HostOnlyPacket(Packet::Id::BaBLEError, initial_packet_code()) {
      m_native_class = "BaBLE";
      m_status = exception.get_type();
      set_status(static_cast<uint8_t>(m_status), false);

      m_message = exception.get_message();
      m_uuid_request = exception.get_uuid_request();
    }

    vector<uint8_t> BaBLEError::serialize(FlatbuffersFormatBuilder& builder) const {
      auto message = builder.CreateString(m_message);

      auto payload = BaBLE::CreateBaBLEError(builder, message);

      return builder.build(payload, BaBLE::Payload::BaBLEError);
    }

    const string BaBLEError::stringify() const {
      stringstream result;

      result << "<BaBLEError> "
             << AbstractPacket::stringify() << ", "
             << "Type: " << BaBLE::EnumNameStatusCode(m_status) << ", "
             << "Message: " << m_message;

      return result.str();
    }

  }

}