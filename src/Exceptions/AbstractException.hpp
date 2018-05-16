#ifndef BABLE_LINUX_ABSTRACTEXCEPTION_HPP
#define BABLE_LINUX_ABSTRACTEXCEPTION_HPP

#include <string>
#include <exception>
#include "../Log/Loggable.hpp"
#include "constants.hpp"
#include "../Log/Log.hpp"

namespace Exceptions {

  class AbstractException : public std::exception, public Loggable {

  public:
    virtual const Exceptions::Type exception_type() const = 0;
    virtual const std::string exception_name() const = 0;

    const char* what() const noexcept override {
      return m_message.c_str();
    };

    std::string uuid_request() const {
      return m_uuid_request;
    };

  protected:
    explicit AbstractException(const std::string& message) {
      m_message = message;
      m_uuid_request = "";
    };

    std::string m_message;
    std::string m_uuid_request;

  };

}

#endif //BABLE_LINUX_ABSTRACTEXCEPTION_HPP
