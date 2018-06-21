#ifndef BABLE_LINUX_ABSTRACTEXCEPTION_HPP
#define BABLE_LINUX_ABSTRACTEXCEPTION_HPP

#include "../Log/Loggable.hpp"
#include "constants.hpp"

namespace Exceptions {

  class AbstractException : public std::exception, public Loggable {

  public:
    virtual const Exceptions::Type get_type() const = 0;
    virtual const std::string get_name() const = 0;

    const char* what() const noexcept override {
      return m_message.c_str();
    };

    std::string get_uuid_request() const {
      return m_uuid_request;
    };

    std::string get_message() const {
      return m_message;
    };

    const std::string stringify() const override {
      return get_name() + ": " + get_message();
    };

  protected:
    explicit AbstractException(const std::string& message, const std::string& uuid_request) {
      m_message = message;
      m_uuid_request = uuid_request;
    };

    std::string m_message;
    std::string m_uuid_request;

  };

}

#endif //BABLE_LINUX_ABSTRACTEXCEPTION_HPP
