#include "BaBLEException.hpp"

using namespace std;

namespace Exceptions {

  BaBLEException::BaBLEException(BaBLE::StatusCode type, const string& message, const string& uuid_request) {
    m_type = type;
    m_message = message;
    m_uuid_request = uuid_request;
  }
  
  const BaBLE::StatusCode BaBLEException::get_type() const {
    return m_type;
  }
  
  string BaBLEException::get_uuid_request() const {
    return m_uuid_request;
  }
  
  string BaBLEException::get_message() const {
    return m_message;
  }

  void BaBLEException::set_uuid_request(const string& uuid_request) {
    m_uuid_request = uuid_request;
  }
  
  const char* BaBLEException::what() const noexcept {
    return m_message.c_str();
  }
  
}