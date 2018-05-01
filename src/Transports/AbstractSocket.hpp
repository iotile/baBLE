#ifndef BABLE_LINUX_ABSTRACTSOCKET_HPP
#define BABLE_LINUX_ABSTRACTSOCKET_HPP

#include <uvw.hpp>
#include "../Packet/AbstractPacket.hpp"
#include "../Packet/constants.hpp"
#include "../Log/Log.hpp"

class AbstractSocket {

public:
  using CallbackFunction = std::function<void(const std::vector<uint8_t>&)>;

  explicit AbstractSocket(std::shared_ptr<AbstractFormat> format) {
    m_format = std::move(format);
  }

  virtual const Packet::Type packet_type() const {
    return m_format->packet_type();
  };

  virtual bool send(const std::vector<uint8_t>& data) = 0;
  virtual void poll(std::shared_ptr<uvw::Loop> loop, CallbackFunction on_received) = 0;

  virtual ~AbstractSocket() = default;

protected:
  std::shared_ptr<AbstractFormat> m_format;

};

#endif //BABLE_LINUX_ABSTRACTSOCKET_HPP
