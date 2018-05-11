#include <memory>
#include <uvw.hpp>
#include "Log/Log.hpp"
#include "Format/Ascii/AsciiFormat.hpp"
#include "Format/MGMT/MGMTFormat.hpp"
#include "Format/Flatbuffers/FlatbuffersFormat.hpp"
#include "Transport/Socket/MGMT/MGMTSocket.hpp"
#include "Transport/Socket/StdIO/StdIOSocket.hpp"
#include "Transport/SocketContainer/SocketContainer.hpp"
#include "Builder/PacketBuilder.hpp"
#include "Exceptions/AbstractException.hpp"
#include "Packet/BaBLEError/BaBLEErrorPacket.hpp"
#include "bootstrap.hpp"

using namespace std;
using namespace uvw;

// Function used to call all handlers closing callbacks before stopping the loop
void cleanly_stop_loop(Loop& loop) {
  loop.walk([](BaseHandle &handle){
    handle.close();
  });
  loop.stop();
  LOG.debug("Handles stopped.");
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

  // Formats
  shared_ptr<MGMTFormat> mgmt_format = make_shared<MGMTFormat>();
  shared_ptr<FlatbuffersFormat> fb_format = make_shared<FlatbuffersFormat>();
  shared_ptr<AsciiFormat> ascii_format = make_shared<AsciiFormat>();

  // Sockets
  shared_ptr<MGMTSocket> mgmt_socket = make_shared<MGMTSocket>(mgmt_format);
  shared_ptr<StdIOSocket> stdio_socket = make_shared<StdIOSocket>(fb_format);

  // Create socket manager
  SocketContainer socket_container;
  socket_container
      .register_socket(mgmt_socket)
      .register_socket(stdio_socket);

  // Builder
  PacketBuilder mgmt_builder(mgmt_format);
  Bootstrap::register_mgmt_packets(mgmt_builder, stdio_socket->format());

  PacketBuilder stdio_builder(stdio_socket->format());
  Bootstrap::register_stdio_packets(stdio_builder, mgmt_socket->format());

  // Poll sockets
  mgmt_socket->poll(loop, [&mgmt_builder, &socket_container, &stdio_socket](const std::vector<uint8_t>& received_data) {
    try {
      std::unique_ptr<Packet::AbstractPacket> packet = mgmt_builder.build(received_data);
      LOG.debug("Packet built", "MGMT poller");

      packet->translate();
      LOG.debug("Packet translated", "MGMT poller");

      socket_container.send(std::move(packet));

    } catch (const Exceptions::AbstractException& err) {
      LOG.error(err.stringify(), "MGMT poller");
      std::unique_ptr<Packet::Errors::BaBLEErrorPacket> error_packet = make_unique<Packet::Errors::BaBLEErrorPacket>(stdio_socket->format()->packet_type());
      error_packet->import(err);
      socket_container.send(std::move(error_packet));
    }
  });

  stdio_socket->poll(loop, [&stdio_builder, &socket_container, &stdio_socket](const std::vector<uint8_t>& received_data) {
    try {
      std::unique_ptr<Packet::AbstractPacket> packet = stdio_builder.build(received_data);
      LOG.debug("Packet built", "BABLE poller");

      packet->translate();
      LOG.debug("Packet translated", "BABLE poller");

      socket_container.send(std::move(packet));

    } catch (const Exceptions::AbstractException& err) {
      LOG.error(err.stringify(), "BABLE poller");
      std::unique_ptr<Packet::Errors::BaBLEErrorPacket> error_packet = make_unique<Packet::Errors::BaBLEErrorPacket>(stdio_socket->format()->packet_type());
      error_packet->import(err);
      socket_container.send(std::move(error_packet));
    }
  });

  // Start the loop
  LOG.info("Start loop...");
  loop->run<Loop::Mode::DEFAULT>();
  LOG.info("Loop stopped.");

  // Close and clean the loop
  loop->close();
  loop->clear();
  return 0;
}
