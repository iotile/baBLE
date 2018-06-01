#ifndef BABLE_LINUX_ABSTRACTSOCKET_HPP
#define BABLE_LINUX_ABSTRACTSOCKET_HPP

#include <functional>
#include <uvw.hpp>
#include "../Format/AbstractFormat.hpp"

class AbstractSocket {

public:
  using OnReceivedCallback = std::function<void(const std::vector<uint8_t>&, const std::shared_ptr<AbstractFormat>& format)>;
  using OnErrorCallback = std::function<void(const Exceptions::AbstractException&)>;

  explicit AbstractSocket(std::shared_ptr<AbstractFormat> format) {
    m_format = std::move(format);
    m_controller_id = NON_CONTROLLER_ID;
  }

  const std::shared_ptr<AbstractFormat> format() const {
    return m_format;
  };

  const uint16_t get_controller_id() const {
    return m_controller_id;
  };

  virtual bool send(const std::vector<uint8_t>& data) = 0;
  virtual void poll(std::shared_ptr<uvw::Loop> loop, OnReceivedCallback on_received, OnErrorCallback on_error) = 0;

  virtual ~AbstractSocket() = default;

protected:
  std::shared_ptr<AbstractFormat> m_format;
  uint16_t m_controller_id;

};

#endif //BABLE_LINUX_ABSTRACTSOCKET_HPP
