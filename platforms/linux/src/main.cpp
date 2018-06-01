#include <memory>
#include <uvw.hpp>
#include "Log/Log.hpp"
#include "Application/PacketBuilder/PacketBuilder.hpp"
#include "Application/PacketRouter/PacketRouter.hpp"
#include "Format/Ascii/AsciiFormat.hpp"
#include "Format/Flatbuffers/FlatbuffersFormat.hpp"
#include "Transport/Socket/MGMT/MGMTSocket.hpp"
#include "Transport/Socket/HCI/HCISocket.hpp"
#include "Transport/Socket/StdIO/StdIOSocket.hpp"
#include "Transport/SocketContainer/SocketContainer.hpp"
#include "Exceptions/AbstractException.hpp"
#include "Exceptions/RuntimeError/RuntimeErrorException.hpp"
#include "Application/Packets/Errors/BaBLEError/BaBLEError.hpp"
#include "Application/Packets/Control/Ready/Ready.hpp"
#include "bootstrap.hpp"

#define EXPIRATION_DURATION_SECONDS 60

using namespace std;
using namespace uvw;

// TODO: clean includes to make cmake faster
// TODO: Merge manufacturer_data_advertised and manufacturer_data_scanned into one variable in DeviceFound packet
// TODO: Add an option to set the logging level (--logging=[debug|info|warning|error|critical])
// TODO: use ioctl and put function into a static create_all function in HCI socket
// TODO: replace bytearray addresses by string address in flatbuffers
// TODO: Create a ScanBLEForever command (remove MGMT StartScan ? -> can't be implemented in Windows and Mac...) /!\ Needs to return ScanResponse AND Advertisments

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
  LOG.debug("Loop created.");

  // Stop on SIGINT signal
  shared_ptr<SignalHandle> stop_signal = loop->resource<SignalHandle>();
  stop_signal->on<SignalEvent>([] (const SignalEvent&, SignalHandle& sig) {
    LOG.warning("Stop signal received...", "Signal");
    cleanly_stop_loop(sig.loop());
  });
  stop_signal->start(SIGINT);
  LOG.debug("SIGINT signal handled.");

  // Formats
  LOG.info("Creating formats...");
  shared_ptr<HCIFormat> hci_format = make_shared<HCIFormat>();
  shared_ptr<MGMTFormat> mgmt_format = make_shared<MGMTFormat>();
  shared_ptr<FlatbuffersFormat> fb_format = make_shared<FlatbuffersFormat>();
  shared_ptr<AsciiFormat> ascii_format = make_shared<AsciiFormat>();

  // Sockets
  LOG.info("Creating sockets...");
  shared_ptr<MGMTSocket> mgmt_socket = make_shared<MGMTSocket>(mgmt_format);
  shared_ptr<StdIOSocket> stdio_socket = make_shared<StdIOSocket>(fb_format);
  vector<shared_ptr<HCISocket>> hci_sockets = Bootstrap::create_hci_sockets(mgmt_socket, hci_format);

  // Socket container
  LOG.info("Registering sockets into socket container...");
  SocketContainer socket_container;
  socket_container
      .register_socket(mgmt_socket)
      .register_socket(stdio_socket);
  for (auto& hci_socket : hci_sockets) {
    socket_container.register_socket(hci_socket);
  }

  // PacketRouter
  shared_ptr<PacketRouter> packet_router = make_shared<PacketRouter>();

  // PacketBuilder
  LOG.info("Registering packets into packet builder...");
  PacketBuilder mgmt_packet_builder(mgmt_format);
  Bootstrap::register_mgmt_packets(mgmt_packet_builder, stdio_socket->format());

  PacketBuilder hci_packet_builder(hci_format);
  Bootstrap::register_hci_packets(hci_packet_builder, stdio_socket->format());

  PacketBuilder stdio_packet_builder(stdio_socket->format());
  Bootstrap::register_stdio_packets(stdio_packet_builder, mgmt_format, hci_format);

  // Poll sockets
  LOG.info("Creating socket pollers...");

  auto on_error = [&stdio_socket, &socket_container](const Exceptions::AbstractException& err) {
    LOG.error(err.stringify(), "Error");
    std::shared_ptr<Packet::Errors::BaBLEError> error_packet = make_shared<Packet::Errors::BaBLEError>(
        stdio_socket->format()->get_packet_type()
    );
    error_packet->from_exception(err);
    socket_container.send(error_packet);
  };

  mgmt_socket->poll(
    loop,
    [&mgmt_packet_builder, &packet_router, &socket_container](const std::vector<uint8_t>& received_data, const std::shared_ptr<AbstractFormat>& format) {
      shared_ptr<AbstractExtractor> extractor = format->create_extractor(received_data);

      std::shared_ptr<Packet::AbstractPacket> packet = mgmt_packet_builder.build(extractor);
      LOG.debug("Packet built", "MGMT poller");

      packet = packet_router->route(packet_router, packet);
      LOG.debug("Packet routed", "MGMT poller");

      packet->before_sent(packet_router);
      LOG.debug("Packet prepared to be sent", "MGMT poller");

      socket_container.send(packet);
      LOG.debug("Packet sent", "MGMT poller");
    },
    on_error
  );

  for (auto& hci_socket : hci_sockets) {
    hci_socket->poll(
      loop,
      [&hci_socket, &hci_packet_builder, &packet_router, &socket_container](const std::vector<uint8_t>& received_data, const std::shared_ptr<AbstractFormat>& format) {
        // Create extractor from raw bytes received
        shared_ptr<AbstractExtractor> extractor = format->create_extractor(received_data);
        extractor->set_controller_id(hci_socket->get_controller_id());

        // Build packet
        std::shared_ptr<Packet::AbstractPacket> packet = hci_packet_builder.build(extractor);
        LOG.debug("Packet built", "HCI poller");

        // This part is needed due to a bug since Linux Kernel v4 : we have to create manually the L2CAP socket, else
        // we'll be disconnected after sending one packet.
        if (packet->get_id() == Packet::Id::DeviceConnected) {
          auto device_connected_packet = std::dynamic_pointer_cast<Packet::Events::DeviceConnected>(packet);
          if (device_connected_packet == nullptr) {
            throw Exceptions::RuntimeErrorException("Can't downcast packet to DeviceConnected packet");
          }

          LOG.debug(*device_connected_packet, "HCI poller - DeviceConnected");
          hci_socket->connect_l2cap_socket(
              device_connected_packet->get_connection_id(),
              device_connected_packet->get_raw_device_address(),
              device_connected_packet->get_device_address_type()
          );
        } else if (packet->get_id() == Packet::Id::DeviceDisconnected) {
          auto device_disconnected_packet = std::dynamic_pointer_cast<Packet::Events::DeviceDisconnected>(packet);
          if (device_disconnected_packet == nullptr) {
            throw Exceptions::RuntimeErrorException("Can't downcast packet to DeviceDisconnected packet");
          }
          LOG.debug(*device_disconnected_packet, "HCI poller - DeviceDisconnected");

          hci_socket->disconnect_l2cap_socket(device_disconnected_packet->get_connection_id());
        }

        // Check if there are packets waiting for a response
        packet = packet_router->route(packet_router, packet);
        LOG.debug("Packet routed", "HCI poller");

        packet->before_sent(packet_router);
        LOG.debug("Packet prepared to be sent", "HCI poller");

        socket_container.send(packet);
        LOG.debug("Packet sent", "HCI poller");
      },
      on_error
    );
  }

  stdio_socket->poll(
    loop,
    [&stdio_packet_builder, &packet_router, &socket_container, &loop](const std::vector<uint8_t>& received_data, const std::shared_ptr<AbstractFormat>& format) {
      shared_ptr<AbstractExtractor> extractor = format->create_extractor(received_data);

      std::shared_ptr<Packet::AbstractPacket> packet = stdio_packet_builder.build(extractor);
      LOG.debug("Packet built", "BABLE poller");

      packet->before_sent(packet_router);
      LOG.debug("Packet prepared to be sent", "BABLE poller");

      socket_container.send(packet);
      LOG.debug("Packet sent", "BABLE poller");

      if (packet->get_id() == Packet::Id::Exit) {
        LOG.debug("Received Exit packet. Cleanly stopping loop...");
        cleanly_stop_loop(*loop);
        return;
      }
    },
    on_error
  );

  // Removed all expired waiting response (> 5 minutes)
  LOG.info("Creating expiration timer...");
  packet_router->start_expiration_timer(loop, EXPIRATION_DURATION_SECONDS);

  // Send Ready packet to indicate that BaBLE has started and is ready to receive commands
  LOG.info("Sending Ready packet...");
  std::shared_ptr<Packet::Control::Ready> ready_packet = make_shared<Packet::Control::Ready>(stdio_socket->format()->get_packet_type());
  socket_container.send(ready_packet);

  // Start the loop
  LOG.info("Starting loop...");
  loop->run<Loop::Mode::DEFAULT>();
  LOG.info("Loop stopped.");

  // Close and clean the loop
  loop->close();
  loop->clear();
  return 0;
}
