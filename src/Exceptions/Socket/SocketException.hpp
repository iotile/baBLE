#ifndef BABLE_LINUX_SOCKETEXCEPTION_HPP
#define BABLE_LINUX_SOCKETEXCEPTION_HPP

#include "../AbstractException.hpp"

namespace Exceptions {

  class SocketException : public AbstractException {

  public:
    explicit SocketException(const std::string& message): AbstractException(message) {};

    const Exceptions::Type exception_type() const override {
      return Exceptions::Type::SocketError;
    };

    const std::string exception_name() const override {
      return "SocketError";
    };

    const std::string stringify() const override {
      return "Socket error: " + m_message;
    };

  };

}

#endif //BABLE_LINUX_SOCKETEXCEPTION_HPP
