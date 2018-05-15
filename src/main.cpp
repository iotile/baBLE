#include <memory>
#include <uvw.hpp>
#include "Log/Log.hpp"
#include "Format/Ascii/AsciiFormat.hpp"
#include "Format/MGMT/MGMTFormat.hpp"
#include "Format/Flatbuffers/FlatbuffersFormat.hpp"
#include "Transport/Socket/MGMT/MGMTSocket.hpp"
#include "Transport/Socket/StdIO/StdIOSocket.hpp"
#include "Transport/SocketContainer/SocketContainer.hpp"
#include "PacketContainer/PacketContainer.hpp"
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

// TODO: now uuid can be added thanks to the new system. Is it needed ? Is it compatible with Mac/Windows implementation ?
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

  // PacketContainer
  PacketContainer mgmt_packet_container(mgmt_format);
  Bootstrap::register_mgmt_packets(mgmt_packet_container, stdio_socket->format());

  PacketContainer stdio_packet_container(stdio_socket->format());
  Bootstrap::register_stdio_packets(stdio_packet_container, mgmt_socket->format());

  // Poll sockets
  mgmt_socket->poll(loop, [&mgmt_packet_container, &socket_container, &stdio_socket](const std::vector<uint8_t>& received_data) {
    try {
      std::shared_ptr<Packet::AbstractPacket> packet = mgmt_packet_container.build(received_data);
      LOG.debug("Packet built", "MGMT poller");

      packet->translate();
      LOG.debug("Packet translated", "MGMT poller");

      if(packet->expected_response_uuid() != 0) {
        PacketContainer::wait_response(packet);
      }

      socket_container.send(packet);

    } catch (const Exceptions::AbstractException& err) {
      LOG.error(err.stringify(), "MGMT poller");
      std::shared_ptr<Packet::Errors::BaBLEErrorPacket> error_packet = make_shared<Packet::Errors::BaBLEErrorPacket>(
          stdio_socket->format()->packet_type()
      );
      error_packet->import(err);
      socket_container.send(error_packet);
    }
  });

  stdio_socket->poll(loop, [&stdio_packet_container, &socket_container, &stdio_socket](const std::vector<uint8_t>& received_data) {
    try {
      std::shared_ptr<Packet::AbstractPacket> packet = stdio_packet_container.build(received_data);
      LOG.debug("Packet built", "BABLE poller");

      packet->translate();
      LOG.debug("Packet translated", "BABLE poller");

      if(packet->expected_response_uuid() != 0) {
        PacketContainer::wait_response(packet);
      }

      socket_container.send(packet);

    } catch (const Exceptions::AbstractException& err) {
      LOG.error(err.stringify(), "BABLE poller");
      std::shared_ptr<Packet::Errors::BaBLEErrorPacket> error_packet = make_shared<Packet::Errors::BaBLEErrorPacket>(
          stdio_socket->format()->packet_type()
      );
      error_packet->import(err);
      socket_container.send(error_packet);
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
