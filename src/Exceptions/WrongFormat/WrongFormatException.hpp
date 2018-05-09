#ifndef BABLE_LINUX_WRONGFORMATEXCEPTION_HPP
#define BABLE_LINUX_WRONGFORMATEXCEPTION_HPP

#include "../AbstractException.hpp"

namespace Exceptions {

  class WrongFormatException : public AbstractException {

  public:
    explicit WrongFormatException(const std::string& message): AbstractException(message) {};

    const Exceptions::Type exception_type() const override {
      return Exceptions::Type::WrongFormat;
    };

    const std::string exception_name() const override {
      return "WrongFormat";
    };

    const std::string stringify() const override {
      return "Wrong format error: " + m_message;
    };

  };

}

#endif //BABLE_LINUX_WRONGFORMATEXCEPTION_HPP
