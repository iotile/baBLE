#include <memory>
#include <uvw.hpp>
#include "Log/Log.hpp"
#include "Format/MGMT/MGMTFormat.hpp"
#include "Format/Flatbuffers/FlatbuffersFormat.hpp"
#include "Transport/Socket/MGMT/MGMTSocket.hpp"
#include "Transport/Socket/StdIO/StdIOSocket.hpp"
#include "Builder/PacketBuilder.hpp"
#include "Packet/Commands/GetMGMTInfo/GetMGMTInfo.hpp"
#include "Packet/Commands/Scan/StartScan.hpp"
#include "Packet/Commands/Scan/StopScan.hpp"
#include "Packet/Events/DeviceFound/DeviceFound.hpp"
#include "Packet/Events/Discovering/Discovering.hpp"
#include "Transport/SocketContainer/SocketContainer.hpp"
#include "Format/Ascii/AsciiFormat.hpp"

using namespace std;
using namespace uvw;

// Function used to call all handlers closing callbacks before stopping the loop
void cleanly_stop_loop(Loop& loop) {
  loop.walk([](BaseHandle &handle){
    handle.close();
  });
  loop.stop();
}

// TUESDAY 1st May =>
// TODO: remove unused includes
// TODO: clean code
// TODO: handle errors properly (if exception OR status in complete event OR status in status event) -> forward to bable socket

// TODO: idea -> put all registration into a bootstap.cpp file with a bootstrap() function
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
  PacketBuilder mgmt_builder(mgmt_format, stdio_socket->format());
  mgmt_builder
      .register_command<Packet::Commands::GetMGMTInfo>()
      .register_command<Packet::Commands::StartScan>()
      .register_command<Packet::Commands::StopScan>()
      .register_event<Packet::Events::DeviceFound>()
      .register_event<Packet::Events::Discovering>();

  PacketBuilder stdio_builder(stdio_socket->format());
  stdio_builder
    .set_output_format(mgmt_socket->format())
      .register_command<Packet::Commands::GetMGMTInfo>()
      .register_command<Packet::Commands::StartScan>()
      .register_command<Packet::Commands::StopScan>();

  // Poll sockets
  mgmt_socket->poll(loop, [&mgmt_builder, &socket_container](const std::vector<uint8_t>& received_data) {
    try {
      std::unique_ptr<Packet::AbstractPacket> packet = mgmt_builder.build(received_data);
      LOG.debug("Packet built", "MGMT poller");
      LOG.debug(*packet, "MGMT poller");
      packet->translate();
      LOG.debug("Packet translated", "MGMT poller");
      socket_container.send(std::move(packet));

    } catch (const exception& err) {
      LOG.error(err.what(), "MGMT poller");
    }
  });

  stdio_socket->poll(loop, [&stdio_builder, &socket_container](const std::vector<uint8_t>& received_data) {
    try {
      std::unique_ptr<Packet::AbstractPacket> packet = stdio_builder.build(received_data);
      LOG.debug("Packet built", "BABLE poller");
      packet->translate();
      LOG.debug("Packet translated", "BABLE poller");
      socket_container.send(std::move(packet));

    } catch (const exception& err) {
      LOG.error(err.what(), "BABLE poller");
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
