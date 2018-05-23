#include "BaBLEErrorPacket.hpp"

using namespace std;

namespace Packet::Errors {

  BaBLEErrorPacket::BaBLEErrorPacket(Packet::Type output_type)
    : AbstractPacket(output_type, output_type) {
    m_id = BaBLE::Payload::BaBLEError;
    m_message = "";
    m_type = Exceptions::Type::Unknown;
    m_native_class = "BaBLE";
  }

  vector<uint8_t> BaBLEErrorPacket::serialize(AsciiFormatBuilder& builder) const {
    builder
      .set_name("BaBLEError")
      .add("Type", m_name)
      .add("Message", m_message);
    return {};
  };

  vector<uint8_t> BaBLEErrorPacket::serialize(FlatbuffersFormatBuilder& builder) const {
    auto name = builder.CreateString(m_name);
    auto message = builder.CreateString(m_message);
    BaBLE::StatusCode status_code;

    switch (m_type) {
      case Exceptions::Type::Unknown:
        status_code = BaBLE::StatusCode::Unknown;
        break;

      case Exceptions::Type::NotFound:
        status_code = BaBLE::StatusCode::NotFound;
        break;

      case Exceptions::Type::SocketError:
        status_code = BaBLE::StatusCode::SocketError;
        break;

      case Exceptions::Type::WrongFormat:
        status_code = BaBLE::StatusCode::WrongFormat;
        break;

      case Exceptions::Type::InvalidCommand:
        status_code = BaBLE::StatusCode::InvalidCommand;
        break;

      case Exceptions::Type::RuntimeError:
        status_code = BaBLE::StatusCode::Failed;
    }

    auto payload = BaBLE::CreateBaBLEError(builder, name, message);

    return builder
        .set_status(status_code)
        .build(payload, BaBLE::Payload::BaBLEError);
  };

  void BaBLEErrorPacket::from_exception(const Exceptions::AbstractException& exception) {
    m_type = exception.get_type();
    m_name = exception.get_name();
    m_message = exception.stringify();
    m_uuid_request = exception.get_uuid_request();
  };

}