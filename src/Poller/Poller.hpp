#ifndef BABLE_LINUX_POLLER_HPP
#define BABLE_LINUX_POLLER_HPP

#include <functional>
#include <memory>
#include <uvw.hpp>
#include "../Socket/AbstractSocket.hpp"

class Poller {
  using CallbackFunction = std::function<void()>;

public:
  Poller(const std::shared_ptr<uvw::Loop>& loop, uvw::OSSocketHandle raw_socket) {
    m_poller = loop->resource<uvw::PollHandle>(raw_socket);
  };

  Poller& on_readable(const CallbackFunction& on_readable) {
    m_on_readable = on_readable;

    return *this;
  }

  Poller& on_writable(const CallbackFunction& on_writable) {
    m_on_writable = on_writable;

    return *this;
  }

  void start() {
    m_poller->on<uvw::PollEvent>([this](const uvw::PollEvent& event, const uvw::PollHandle& handle){
      if (event.flags & uvw::PollHandle::Event::READABLE) {
        m_on_readable();
      } else if (event.flags & uvw::PollHandle::Event::WRITABLE) {
        m_on_writable();
      }
    });

    uvw::Flags<uvw::PollHandle::Event> readable = uvw::Flags<uvw::PollHandle::Event>(uvw::PollHandle::Event::READABLE);
    uvw::Flags<uvw::PollHandle::Event> writable = uvw::Flags<uvw::PollHandle::Event>(uvw::PollHandle::Event::WRITABLE);
    m_poller->start(readable | writable);
  }

private:
  std::shared_ptr<uvw::PollHandle> m_poller;

  CallbackFunction m_on_readable;
  CallbackFunction m_on_writable;

};

#endif //BABLE_LINUX_POLLER_HPP
