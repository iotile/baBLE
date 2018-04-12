#include <memory>
#include <uvw.hpp>
#include "Log/Log.hpp"
#include "MGMTSocket.hpp"

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
  loop->on<ErrorEvent>([](const ErrorEvent& err, Loop& l) {
    LOG.error("An error occured: " + string(err.what()), "Loop");
  });

  // Stop on SIGINT signal
  shared_ptr<SignalHandle> stop_signal = loop->resource<SignalHandle>();
  stop_signal->on<SignalEvent>([](const SignalEvent&, SignalHandle& sig) {
    LOG.warning("Stop signal received...", "Signal");
    cleanly_stop_loop(sig.loop());
  });
  stop_signal->start(SIGINT);

  // Create MGMT socket
  shared_ptr<MGMTSocket> mgmt_sock = make_shared<MGMTSocket>();

  // Poll the MGMT socket on READABLE (data available) and WRITABLE (we can write on the socket)
  shared_ptr<PollHandle> sock_poll = loop->resource<PollHandle>(mgmt_sock->get_socket());
  sock_poll->on<PollEvent>([&mgmt_sock] (const PollEvent& event, const PollHandle& handle) {
    if (event.flags & PollHandle::Event::READABLE) {
      printf("DATA RECEIVED !\n");
      Deserializer deser = mgmt_sock->receive();
      LOG.debug(deser, "MGMT poller");

      uint16_t event_code;
      uint16_t controller_index;
      uint16_t payload_length;
      uint16_t command_code;
      uint8_t status;

      uint8_t version;
      uint16_t revision;
      deser >> event_code >> controller_index >> payload_length >> command_code >> status >> version >> revision;
      printf("Event: %04x\n"
             "Controller: %04x\n"
             "Length: %04x\n"
             "Command code: %04x\n"
             "Status: %02x\n"
             "Version: %02x\n"
             "Revision: %04x\n", event_code, controller_index, payload_length, command_code, status, version, revision);
    } else if (event.flags & PollHandle::Event::WRITABLE) {
      mgmt_sock->set_writable();
    }
  });
  sock_poll->start(Flags<PollHandle::Event>(PollHandle::Event::READABLE) | Flags<PollHandle::Event>(PollHandle::Event::WRITABLE));

  // Run a timer calling callback every second
  int counter = 0;
  shared_ptr<TimerHandle> timer = loop->resource<TimerHandle>();
  timer->on<TimerEvent>([&counter, &mgmt_sock](const TimerEvent&, TimerHandle& t) {
    counter++;
    LOG.debug("TIC #" + to_string(counter), "Timer");

    if (counter == 2) {
      Serializer ser;
      uint16_t command_code = 0x0001;
      uint16_t controller_id = 0xFFFF;
      uint16_t params_length = 0;
      ser << command_code << controller_id << params_length;
      LOG.debug(ser, "Timer");

      Serializer ser2 = ser;
      LOG.debug(ser2, "Timer");
      cout << ser2.stringify();
      mgmt_sock->send(ser);
      mgmt_sock->send(ser2);
    }

    if (counter == 5) {
      cleanly_stop_loop(t.loop());
    }
  });
  timer->start(chrono::seconds(1), chrono::seconds(1));

  // Start the loop
  LOG.info("Start loop...");
  loop->run<Loop::Mode::DEFAULT>();
  LOG.info("Loop stopped.");

  // Close and clean the loop
  loop->close();
  loop->clear();
  return 0;
}