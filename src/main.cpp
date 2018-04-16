#include <memory>
#include <uvw.hpp>
#include "Log/Log.hpp"
#include "Socket/MGMTSocket.hpp"
#include "Commands/CommandManager.hpp"

using namespace std;
using namespace uvw;

// Function used to call all handlers closing callbacks before stopping the loop
void cleanly_stop_loop(Loop& loop) {
  loop.walk([](BaseHandle &handle){
    handle.close();
  });
  loop.stop();
}

int main() {
  ENABLE_LOGGING(DEBUG);

  // Create loop
  shared_ptr<Loop> loop = Loop::getDefault();
  loop->on<ErrorEvent>([] (const ErrorEvent& err, Loop& l) {
    LOG.error("An error occured: " + string(err.what()), "Loop");
  });

  // Stop on SIGINT signal
  shared_ptr<SignalHandle> stop_signal = loop->resource<SignalHandle>();
  stop_signal->on<SignalEvent>([] (const SignalEvent&, SignalHandle& sig) {
    LOG.warning("Stop signal received...", "Signal");
    cleanly_stop_loop(sig.loop());
  });
  stop_signal->start(SIGINT);

  // Create MGMT socket
  shared_ptr<MGMTSocket> mgmt_sock = make_shared<MGMTSocket>();

  shared_ptr<PipeHandle> stdin_pipe = loop->resource<PipeHandle>(false);
  stdin_pipe->data(mgmt_sock);
  stdin_pipe->open(0); // 0 is stdin
  stdin_pipe->read();
  stdin_pipe->on<DataEvent>([] (const DataEvent& event, const PipeHandle& handle) {
    const string cmd(event.data.get());
    LOG.info("Data on stdin: " + cmd);

    if (!cmd.empty()) {
      shared_ptr<MGMTSocket> sock = handle.data<MGMTSocket>();
      CommandManager::Process(cmd, sock); // create command and send it to MGMT socket
    }
  });

  // Poll the MGMT socket on READABLE (data available) and WRITABLE (we can write on the socket)
  shared_ptr<PollHandle> sock_poll = loop->resource<PollHandle>(mgmt_sock->get_socket());
  sock_poll->data(mgmt_sock);
  sock_poll->on<PollEvent>([] (const PollEvent& event, const PollHandle& handle) {
    shared_ptr<MGMTSocket> sock = handle.data<MGMTSocket>();

    if (event.flags & PollHandle::Event::READABLE) {
      Deserializer deser = sock->receive();
      LOG.debug(deser, "MGMT poller");
      CommandManager::Process(deser, stdout); // Read MGMT response and send data to stdout (via command class)

    } else if (event.flags & PollHandle::Event::WRITABLE) {
      sock->set_writable(true);
    }
  });
  sock_poll->start(Flags<PollHandle::Event>(PollHandle::Event::READABLE) | Flags<PollHandle::Event>(PollHandle::Event::WRITABLE));

  // Run a timer calling callback every second
//  int counter = 0;
//  shared_ptr<TimerHandle> timer = loop->resource<TimerHandle>();
//  timer->on<TimerEvent>([&counter](const TimerEvent&, TimerHandle& t) {
//    counter++;
//    LOG.debug("TIC #" + to_string(counter), "Timer");
//
//    if (counter == 5) {
//      cleanly_stop_loop(t.loop());
//    }
//  });
//  timer->start(chrono::seconds(1), chrono::seconds(1));

  // Start the loop
  LOG.info("Start loop...");
  loop->run<Loop::Mode::DEFAULT>();
  LOG.info("Loop stopped.");

  // Close and clean the loop
  loop->close();
  loop->clear();
  return 0;
}