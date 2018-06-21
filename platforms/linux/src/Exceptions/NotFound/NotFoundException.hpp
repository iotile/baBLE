#ifndef BABLE_LINUX_NOTFOUNDEXCEPTION_HPP
#define BABLE_LINUX_NOTFOUNDEXCEPTION_HPP

#include "../AbstractException.hpp"

namespace Exceptions {

  class NotFoundException : public AbstractException {

  public:
    explicit NotFoundException(const std::string& message, const std::string& uuid_request = "")
        : AbstractException(message, uuid_request) {};

    const Exceptions::Type get_type() const override {
      return Exceptions::Type::NotFound;
    };

    const std::string get_name() const override {
      return "NotFound";
    };

  };

}

#endif //BABLE_LINUX_NOTFOUNDEXCEPTION_HPP
