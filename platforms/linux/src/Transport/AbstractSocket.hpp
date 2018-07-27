#ifndef BABLE_ABSTRACTSOCKET_HPP
#define BABLE_ABSTRACTSOCKET_HPP

#include "Format/AbstractFormat.hpp"
#include "Exceptions/BaBLEException.hpp"

class AbstractSocket {

public:
  using OnReceivedCallback = std::function<void(const std::vector<uint8_t>&, AbstractSocket*)>;
  using OnErrorCallback = std::function<void(const Exceptions::BaBLEException&)>;

  explicit AbstractSocket(std::shared_ptr<AbstractFormat> format) {
    m_format = std::move(format);
    m_controller_id = NON_CONTROLLER_ID;
    m_poll_started = false;

    m_on_received = [](const std::vector<uint8_t>& data, AbstractSocket* socket) {};
    m_on_error = [](const Exceptions::BaBLEException&) {};
  };

  const std::shared_ptr<AbstractFormat> format() const {
    return m_format;
  };

  const uint16_t get_controller_id() const {
    return m_controller_id;
  };

  virtual bool send(const std::vector<uint8_t>& data, uint16_t connection_handle) = 0;
  virtual void poll(OnReceivedCallback on_received, OnErrorCallback on_error) = 0;

  virtual void close() = 0;

  virtual ~AbstractSocket() = default;

protected:
  std::shared_ptr<AbstractFormat> m_format;
  uint16_t m_controller_id;
  OnReceivedCallback m_on_received;
  OnErrorCallback m_on_error;

  bool m_poll_started;

};

#endif //BABLE_ABSTRACTSOCKET_HPP
