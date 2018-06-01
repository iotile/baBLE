#include "BaBLEError.hpp"

using namespace std;

namespace Packet {

  namespace Errors {

    BaBLEError::BaBLEError(Packet::Type output_type)
        : AbstractPacket(output_type, output_type) {
      m_id = Packet::Id::BaBLEError;
      m_packet_code = static_cast<uint16_t>(BaBLE::Payload::BaBLEError);
      m_message = "";
      m_type = Exceptions::Type::Unknown;
      m_native_class = "BaBLE";
    }

    vector<uint8_t> BaBLEError::serialize(AsciiFormatBuilder& builder) const {
      builder
          .set_name("BaBLEError")
          .add("Type", m_name)
          .add("Message", m_message);
      return {};
    };

    vector<uint8_t> BaBLEError::serialize(FlatbuffersFormatBuilder& builder) const {
      auto name = builder.CreateString(m_name);
      auto message = builder.CreateString(m_message);

      auto payload = BaBLE::CreateBaBLEError(builder, name, message);

      return builder.build(payload, BaBLE::Payload::BaBLEError);
    };

    void BaBLEError::from_exception(const Exceptions::AbstractException& exception) {
      m_type = exception.get_type();
      m_name = exception.get_name();
      m_message = exception.stringify();
      m_uuid_request = exception.get_uuid_request();

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
    };

  }

}