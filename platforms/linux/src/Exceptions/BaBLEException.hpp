#ifndef BABLE_BABLEEXCEPTION_HPP
#define BABLE_BABLEEXCEPTION_HPP

#include <Packet_generated.h>

namespace Exceptions {

  class BaBLEException : public std::exception {

  public:
    explicit BaBLEException(BaBLE::StatusCode type, const std::string& message, const std::string& uuid_request = "");

    const BaBLE::StatusCode get_type() const;
    std::string get_uuid_request() const;
    std::string get_message() const;

    void set_uuid_request(const std::string& uuid_request);

    const char* what() const noexcept override;

  protected:
    BaBLE::StatusCode m_type;
    std::string m_message;
    std::string m_uuid_request;

  };

}

#endif //BABLE_BABLEEXCEPTION_HPP
