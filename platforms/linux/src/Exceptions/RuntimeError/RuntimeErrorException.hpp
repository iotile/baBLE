#ifndef BABLE_LINUX_RUNTIMEERROREXCEPTION_HPP
#define BABLE_LINUX_RUNTIMEERROREXCEPTION_HPP

#include "../AbstractException.hpp"

namespace Exceptions {

  class RuntimeErrorException : public AbstractException {

  public:
    explicit RuntimeErrorException(const std::string& message, const std::string& uuid_request = "")
        : AbstractException(message, uuid_request) {};

    const Exceptions::Type get_type() const override {
      return Exceptions::Type::RuntimeError;
    };

    const std::string get_name() const override {
      return "RuntimeError";
    };

  };

}

#endif //BABLE_LINUX_RUNTIMEERROREXCEPTION_HPP
