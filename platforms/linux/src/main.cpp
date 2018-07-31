#include <uv.h>
#include <csignal>
#include "Log/Log.hpp"
#include "Application/PacketBuilder/PacketBuilder.hpp"
#include "Application/PacketRouter/PacketRouter.hpp"
#include "Transport/Socket/MGMT/MGMTSocket.hpp"
#include "Transport/Socket/HCI/HCISocket.hpp"
#include "Transport/Socket/StdIO/StdIOSocket.hpp"
#include "Transport/SocketContainer/SocketContainer.hpp"
#include "Application/Packets/Errors/BaBLEError/BaBLEError.hpp"
#include "Application/Packets/Control/Ready/Ready.hpp"
#include "Exceptions/BaBLEException.hpp"
#include "utils/options_parser.hpp"
#include "registration.hpp"

using namespace std;

// Function used to call all handlers closing callbacks before stopping the loop
void cleanly_stop_loop(uv_loop_t* loop) {
  uv_walk(loop, [](uv_handle_t* handle, void* arg){
    uv_close(handle, [](uv_handle_t* handle) {});
  }, nullptr);

  uv_stop(loop);
  LOG.debug("Handles stopped.");
}

int main(int argc, char* argv[]) {
  map<string, int> options = Utils::parse_options(argc, argv);

  // Create loop
  uv_loop_t* loop = uv_default_loop();
  uv_loop_init(loop);
  LOG.debug("Loop created.");

  // Handle SIGINT signal
  uv_signal_t stop_signal;
  if (options.at("handle_sigint")) {
    uv_signal_init(loop, &stop_signal);
    uv_signal_start(&stop_signal, [](uv_signal_t* handle, int signum) {
      LOG.warning("Stop signal received...", "Signal");
      cleanly_stop_loop(handle->loop);
    }, SIGINT);
    LOG.debug("SIGINT signal handled");
  } else {
    signal(SIGINT, SIG_IGN);
    LOG.debug("SIGINT signal ignored");
  }

  // Formats
  LOG.debug("Creating formats...");
  shared_ptr<HCIFormat> hci_format = make_shared<HCIFormat>();
  shared_ptr<MGMTFormat> mgmt_format = make_shared<MGMTFormat>();
  shared_ptr<FlatbuffersFormat> fb_format = make_shared<FlatbuffersFormat>();

  // Sockets
  LOG.debug("Creating sockets...");
  shared_ptr<MGMTSocket> mgmt_socket = make_shared<MGMTSocket>(loop, mgmt_format);
  shared_ptr<StdIOSocket> stdio_socket = make_shared<StdIOSocket>(loop, fb_format);
  vector<shared_ptr<HCISocket>> hci_sockets;
  if (options.at("controller_id") < 0) {
    hci_sockets = HCISocket::create_all(loop, hci_format);
  } else {
    hci_sockets.push_back(make_shared<HCISocket>(loop, hci_format, options.at("controller_id")));
  }

  // Socket container
  LOG.debug("Registering sockets into socket container...");
  SocketContainer socket_container;
  socket_container
      .register_socket(mgmt_socket)
      .register_socket(stdio_socket);
  for (auto& hci_socket : hci_sockets) {
    socket_container.register_socket(hci_socket);
  }

  // PacketRouter
  shared_ptr<PacketRouter> packet_router = make_shared<PacketRouter>(loop);

  // PacketBuilder
  LOG.debug("Registering packets into packet builder...");
  PacketBuilder mgmt_packet_builder(mgmt_format);
  register_mgmt_packets(mgmt_packet_builder);

  PacketBuilder hci_packet_builder(hci_format);
  register_hci_packets(hci_packet_builder);

  PacketBuilder stdio_packet_builder(fb_format);
  register_stdio_packets(stdio_packet_builder);

  // Poll sockets
  LOG.debug("Creating socket pollers...");

  auto on_error = [&stdio_socket, &socket_container](const Exceptions::BaBLEException& err) {
    LOG.debug(err.get_message(), "Error");
    shared_ptr<Packet::Errors::BaBLEError> error_packet = make_shared<Packet::Errors::BaBLEError>(err);
    socket_container.send(error_packet);
  };

  mgmt_socket->poll(
    [&mgmt_packet_builder, &packet_router, &socket_container](const vector<uint8_t>& received_data, AbstractSocket* socket) {
      shared_ptr<AbstractExtractor> extractor = socket->format()->create_extractor(received_data);

      shared_ptr<Packet::AbstractPacket> packet = mgmt_packet_builder.build(extractor);
      if (packet == nullptr) {
        return;
      }

      packet = packet_router->route(packet_router, packet);
      packet->prepare(packet_router);

      socket_container.send(packet);
    },
    on_error
  );

  for (auto& hci_socket : hci_sockets) {
    hci_socket->poll(
      [&hci_packet_builder, &packet_router, &socket_container](const vector<uint8_t>& received_data, AbstractSocket* socket) {
        // Create extractor from raw bytes received
        shared_ptr<AbstractExtractor> extractor = socket->format()->create_extractor(received_data);
        extractor->set_controller_id(socket->get_controller_id());

        // Build packet
        shared_ptr<Packet::AbstractPacket> packet = hci_packet_builder.build(extractor);
        if (packet == nullptr) {
          return;
        }

        packet->set_socket(socket);

        // Check if there are packets waiting for a response
        packet = packet_router->route(packet_router, packet);
        packet->prepare(packet_router);

        socket_container.send(packet);
      },
      on_error
    );
  }

  stdio_socket->poll(
    [&stdio_packet_builder, &packet_router, &socket_container, &loop](const vector<uint8_t>& received_data, AbstractSocket* socket) {
      shared_ptr<AbstractExtractor> extractor = socket->format()->create_extractor(received_data);

      shared_ptr<Packet::AbstractPacket> packet = stdio_packet_builder.build(extractor);
      if (packet == nullptr) {
        return;
      }

      Packet::Id packet_id = packet->get_id();

      if (packet_id == Packet::Id::SetGATTTable) {
        shared_ptr<AbstractSocket> base_socket = socket_container.get_socket(Packet::Type::HCI, packet->get_controller_id());
        packet->set_socket(dynamic_cast<HCISocket*>(base_socket.get()));
      } else {
        packet->set_socket(socket);
      }

      // Check if there are packets waiting for a response
      packet = packet_router->route(packet_router, packet);
      packet->prepare(packet_router);

      socket_container.send(packet);

      if (packet_id == Packet::Id::WriteWithoutResponse || packet_id == Packet::Id::EmitNotification) {
        packet->prepare(packet_router);
        socket_container.send(packet);
      }
    },
    on_error
  );
  stdio_socket->on_close([&loop]() {
    cleanly_stop_loop(loop);
  });

  // Removed all expired waiting response
  LOG.debug("Creating expiration timer...");
  packet_router->start_expiration_timer(static_cast<uint64_t>(options.at("expiration_time")));

  // Send Ready packet to indicate that BaBLE has started and is ready to receive commands
  LOG.debug("Sending Ready packet...");
  shared_ptr<Packet::Control::Ready> ready_packet = make_shared<Packet::Control::Ready>();
  socket_container.send(ready_packet);

  // Start the loop
  LOG.info("Starting baBLE bridge loop...");
  uv_run(loop, UV_RUN_DEFAULT);
  LOG.info("Loop stopped");

  // Close and clean the loop
  int result = uv_loop_close(loop);
  if (result != 0) {
    LOG.critical("Failed to close libuv loop: " + string(uv_err_name(result)));
  }

  return 0;
}
