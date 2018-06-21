#ifndef BABLE_LINUX_WRONGFORMATEXCEPTION_HPP
#define BABLE_LINUX_WRONGFORMATEXCEPTION_HPP

#include "../AbstractException.hpp"

namespace Exceptions {

  class WrongFormatException : public AbstractException {

  public:
    explicit WrongFormatException(const std::string& message, const std::string& uuid_request = "")
        : AbstractException(message, uuid_request) {};

    const Exceptions::Type get_type() const override {
      return Exceptions::Type::WrongFormat;
    };

    const std::string get_name() const override {
      return "WrongFormat";
    };

  };

}

#endif //BABLE_LINUX_WRONGFORMATEXCEPTION_HPP
