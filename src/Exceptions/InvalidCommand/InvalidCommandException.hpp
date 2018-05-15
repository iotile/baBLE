#ifndef BABLE_LINUX_INVALIDCOMMANDEXCEPTION_HPP
#define BABLE_LINUX_INVALIDCOMMANDEXCEPTION_HPP

#include "../AbstractException.hpp"

namespace Exceptions {

  class InvalidCommandException : public AbstractException {

  public:
    explicit InvalidCommandException(const std::string& message, const std::string& uuid_request = ""): AbstractException(message) {
      m_uuid_request = uuid_request;
    };

    const Exceptions::Type exception_type() const override {
      return Exceptions::Type::InvalidCommand;
    };

    const std::string exception_name() const override {
      return "InvalidCommand";
    };

    const std::string stringify() const override {
      return "Invalid command error: " + m_message;
    };

  };

}

#endif //BABLE_LINUX_INVALIDCOMMANDEXCEPTION_HPP
