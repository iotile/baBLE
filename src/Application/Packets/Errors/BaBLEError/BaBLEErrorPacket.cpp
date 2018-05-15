#include "BaBLEErrorPacket.hpp"

using namespace std;

namespace Packet::Errors {

  BaBLEErrorPacket::BaBLEErrorPacket(Packet::Type output_type)
    : AbstractPacket(output_type, output_type) {
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
    auto message = builder.CreateString(m_message);
    Schemas::StatusCode status_code;

    switch (m_type) {
      case Exceptions::Type::Unknown:
        status_code = Schemas::StatusCode::Unknown;
        break;

      case Exceptions::Type::NotFound:
        status_code = Schemas::StatusCode::NotFound;
        break;

      case Exceptions::Type::SocketError:
        status_code = Schemas::StatusCode::SocketError;
        break;

      case Exceptions::Type::WrongFormat:
        status_code = Schemas::StatusCode::WrongFormat;
        break;

      case Exceptions::Type::InvalidCommand:
        status_code = Schemas::StatusCode::InvalidCommand;
        break;
    }

    auto payload = Schemas::CreateBaBLEError(builder, message);

    return builder
        .set_status(status_code)
        .build(payload, Schemas::Payload::BaBLEError);
  };

  void BaBLEErrorPacket::import(const Exceptions::AbstractException& exception) {
    m_type = exception.exception_type();
    m_name = exception.exception_name();
    m_message = exception.stringify();
    m_uuid_request = exception.uuid_request();
  };

}