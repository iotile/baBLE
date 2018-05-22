#ifndef BABLE_LINUX_NOTFOUNDEXCEPTION_HPP
#define BABLE_LINUX_NOTFOUNDEXCEPTION_HPP

#include "../AbstractException.hpp"

namespace Exceptions {

  class NotFoundException : public AbstractException {

  public:
    explicit NotFoundException(const std::string& message): AbstractException(message) {};

    const Exceptions::Type get_type() const override {
      return Exceptions::Type::NotFound;
    };

    const std::string get_name() const override {
      return "NotFound";
    };

    const std::string stringify() const override {
      return "Not found error: " + m_message;
    };

  };

}

#endif //BABLE_LINUX_NOTFOUNDEXCEPTION_HPP
