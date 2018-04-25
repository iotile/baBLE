#include <memory>
#include <uvw.hpp>
#include "Log/Log.hpp"
#include "Socket/SocketManager.hpp"
#include "Socket/MGMT/MGMTSocket.hpp"
#include "Socket/StdIO/StdIOSocket.hpp"
#include "Builder/Ascii/AsciiBuilder.hpp"
#include "Builder/MGMT/MGMTBuilder.hpp"
#include "Packet/constants.hpp"
#include "Packet/Responses/GetMGMTInfo/GetMGMTInfo.hpp"
#include "Packet/Commands/GetMGMTInfo/GetMGMTInfo.hpp"
#include "Poller/Poller.hpp"
#include "Poller/PipePoller.hpp"

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

  // Sockets
  shared_ptr<StdIOSocket> bable_socket = make_shared<StdIOSocket>(Packet::Type::ASCII);
  shared_ptr<MGMTSocket> mgmt_socket = make_shared<MGMTSocket>();

  // Builders
  AsciiBuilder ascii_builder;
  ascii_builder.register_packet<Packet::Commands::GetMGMTInfo>(Packet::Type::MGMT);

  MGMTBuilder mgmt_builder;
  mgmt_builder.register_packet<Packet::Responses::GetMGMTInfo>(Packet::Type::ASCII);

  // Create socket manager
  SocketManager socket_manager;
  socket_manager
      .register_socket(mgmt_socket)
      .register_socket(bable_socket);

  // Create pollers
  Poller mgmt_poller(loop, mgmt_socket->get_socket());
  mgmt_poller
      .on_readable([&mgmt_socket, &mgmt_builder, &socket_manager]() {
        LOG.debug("Readable data on MGMT socket...", "MGMT poller");
        Deserializer deser = mgmt_socket->receive();
        LOG.debug(deser, "MGMT poller");
        std::unique_ptr<Packet::AbstractPacket> packet = mgmt_builder.build(deser);
        LOG.debug("Packet built", "MGMT poller");
        packet->translate();
        LOG.debug("Packet translated", "MGMT poller");
        socket_manager.send(std::move(packet));
      })
      .on_writable([&mgmt_socket]() {
        mgmt_socket->set_writable(true);
      })
      .start();

  PipePoller stdinput_poller(loop, STDIO_ID::in);
  stdinput_poller
      .on_data([&bable_socket, &ascii_builder, &socket_manager](const char* data, size_t length) {
        try {
          LOG.debug("Readable data on BaBLE pipe...", "BABLE poller");
          Deserializer deser = bable_socket->receive(data, length);
          LOG.debug(deser, "BABLE poller");
          std::unique_ptr<Packet::AbstractPacket> packet = ascii_builder.build(deser);
          LOG.debug("Packet built", "BABLE poller");
          packet->translate();
          LOG.debug("Packet translated", "BABLE poller");
          socket_manager.send(std::move(packet));

        } catch (const invalid_argument& err) {
          LOG.error(err.what());
        }
      })
      .start();

  // Start the loop
  LOG.info("Start loop...");
  loop->run<Loop::Mode::DEFAULT>();
  LOG.info("Loop stopped.");

  // Close and clean the loop
  loop->close();
  loop->clear();
  return 0;
}
