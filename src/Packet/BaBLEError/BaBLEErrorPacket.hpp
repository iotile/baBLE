#ifndef BABLE_LINUX_BABLEERRORPACKET_HPP
#define BABLE_LINUX_BABLEERRORPACKET_HPP

#include "../AbstractPacket.hpp"
#include "../../Exceptions/AbstractException.hpp"

namespace Packet::Errors {

  class BaBLEErrorPacket : public AbstractPacket {

  public:
    explicit BaBLEErrorPacket(Packet::Type output_type)
        : AbstractPacket(output_type, output_type) {
      m_message = "";
      m_type = Exceptions::Type::Unknown;
    };

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override {
      builder
          .set_name("BaBLEError")
          .add("Type", m_name)
          .add("Message", m_message);
      return {};
    };

    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override {
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

      return builder.build(payload, Schemas::Payload::BaBLEError, "BaBLE", status_code);
    };

    void import(const Exceptions::AbstractException& exception) {
      m_type = exception.exception_type();
      m_name = exception.exception_name();
      m_message = exception.stringify();
    };

  private:
    Exceptions::Type m_type;
    std::string m_message;
    std::string m_name;

  };

}

#endif //BABLE_LINUX_BABLEERRORPACKET_HPP
