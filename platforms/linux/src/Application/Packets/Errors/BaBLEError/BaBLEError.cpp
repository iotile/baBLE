#include <sstream>
#include "BaBLEError.hpp"

using namespace std;

namespace Packet {

  namespace Errors {

    BaBLEError::BaBLEError(const Exceptions::AbstractException& exception)
        : HostOnlyPacket(Packet::Id::BaBLEError, initial_packet_code()) {
      m_native_class = "BaBLE";

      m_type = exception.get_type();
      m_name = exception.get_name();
      m_message = exception.get_message();
      m_uuid_request = exception.get_uuid_request();
      set_status(static_cast<uint8_t>(m_type), false);

      switch (m_type) {
        case Exceptions::Type::Unknown:
          m_status = BaBLE::StatusCode::Unknown;
          break;

        case Exceptions::Type::NotFound:
          m_status = BaBLE::StatusCode::NotFound;
          break;

        case Exceptions::Type::SocketError:
          m_status = BaBLE::StatusCode::SocketError;
          break;

        case Exceptions::Type::WrongFormat:
          m_status = BaBLE::StatusCode::WrongFormat;
          break;

        case Exceptions::Type::InvalidCommand:
          m_status = BaBLE::StatusCode::InvalidCommand;
          break;

        case Exceptions::Type::RuntimeError:
          m_status = BaBLE::StatusCode::Failed;
      }
    }

    vector<uint8_t> BaBLEError::serialize(FlatbuffersFormatBuilder& builder) const {
      auto name = builder.CreateString(m_name);
      auto message = builder.CreateString(m_message);

      auto payload = BaBLE::CreateBaBLEError(builder, name, message);

      return builder.build(payload, BaBLE::Payload::BaBLEError);
    }

    const string BaBLEError::stringify() const {
      stringstream result;

      result << "<BaBLEError> "
             << AbstractPacket::stringify() << ", "
             << "Type: " << m_name << ", "
             << "Message: " << m_message;

      return result.str();
    }

  }

}