#ifndef BABLE_LINUX_PIPEPOLLER_HPP
#define BABLE_LINUX_PIPEPOLLER_HPP

#include <functional>
#include <uvw.hpp>
#include <iostream>

enum STDIO_ID {
  in = 0,
  out = 1
};

class PipePoller {

  using CallbackFunction = std::function<void(const char*, size_t)>;

public:
  PipePoller(const std::shared_ptr<uvw::Loop>& loop, STDIO_ID io) {
    m_poller = loop->resource<uvw::PipeHandle>(false);
    m_poller->open(io);
  };

  PipePoller& on_data(const CallbackFunction& on_data) {
    m_on_data = on_data;

    return *this;
  }

  void start() {
    m_poller->on<uvw::DataEvent>([this](const uvw::DataEvent& event, const uvw::PipeHandle& handle){
      m_on_data(event.data.get(), event.length);
    });

    m_poller->read();
  }

private:
  std::shared_ptr<uvw::PipeHandle> m_poller;

  CallbackFunction m_on_data;
};

#endif //BABLE_LINUX_PIPEPOLLER_HPP
