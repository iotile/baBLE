#include <catch.hpp>
#include <uv.h>
#include "utils/string_formats.hpp"
#include "Transport/SocketContainer/SocketContainer.hpp"
#include "Application/PacketBuilder/PacketBuilder.hpp"
#include "Application/Packets/Meta/GetControllersList/GetControllersList.hpp"
#include "Application/Packets/Commands/GetControllersIds/GetControllersIdsResponse.hpp"
#include "Application/Packets/Commands/GetControllerInfo/GetControllerInfoResponse.hpp"
#include "Application/Packets/Commands/Read/Central/ReadRequest.hpp"
#include "Application/Packets/Commands/Read/Central/ReadResponse.hpp"
#include "Application/Packets/Commands/Write/WriteResponse.hpp"
#include "Application/Packets/Control/Ready/Ready.hpp"
#include "mocks/mock_socket.hpp"
#include "mocks/mock_stdio_socket.hpp"
#include "mocks/mock_hci_socket.hpp"
#include "mocks/mock_mgmt_socket.hpp"

using namespace std;

// Test a full use case sending Flatbuffers and MGMT packets, and receiving Flatbuffers packets
TEST_CASE("Integration (with mocked socket) - MGMT meta packet", "[integration][mgmt]") {
  // Create loop
  uv_loop_t* loop = uv_default_loop();
  uv_loop_init(loop);

  // Formats
  shared_ptr<MGMTFormat> mgmt_format = make_shared<MGMTFormat>();
  shared_ptr<FlatbuffersFormat> fb_format = make_shared<FlatbuffersFormat>();

  // Mocked sockets
  shared_ptr<MockMGMTSocket> mgmt_socket = make_shared<MockMGMTSocket>(loop, mgmt_format);
  shared_ptr<MockStdIOSocket> stdio_socket = make_shared<MockStdIOSocket>(loop, fb_format);
  REQUIRE(mgmt_socket->get_raw()->is_open());
  REQUIRE(mgmt_socket->get_raw()->is_option_set() == false);
  REQUIRE(mgmt_socket->get_raw()->is_binded());
  REQUIRE(mgmt_socket->get_raw()->is_connected() == false);

  // Socket container
  SocketContainer socket_container;
  socket_container
      .register_socket(mgmt_socket)
      .register_socket(stdio_socket);

  // PacketRouter
  shared_ptr<PacketRouter> packet_router = make_shared<PacketRouter>(loop);

  // PacketBuilder
  PacketBuilder mgmt_packet_builder(mgmt_format);
  mgmt_packet_builder
      .register_command<Packet::Commands::GetControllersIdsResponse>()
      .register_command<Packet::Commands::GetControllerInfoResponse>();

  PacketBuilder stdio_packet_builder(fb_format);
  stdio_packet_builder
      .register_command<Packet::Meta::GetControllersList>();

  // Callback called if an error occured in polling function
  auto on_error = [&stdio_socket, &socket_container](const Exceptions::BaBLEException& err) {
    FAIL("Error received");
  };

  mgmt_socket->poll(
      [&mgmt_packet_builder, &packet_router, &socket_container](const vector<uint8_t>& received_data, AbstractSocket* socket) {
        shared_ptr<AbstractExtractor> extractor = socket->format()->create_extractor(received_data);

        shared_ptr<Packet::AbstractPacket> packet = mgmt_packet_builder.build(extractor);
        if (packet == nullptr) {
          FAIL("Can't build packet in MGMT poller");
          return;
        }
        packet = packet_router->route(packet_router, packet);
        packet->prepare(packet_router);
        socket_container.send(packet);
      },
      on_error
  );

  stdio_socket->poll(
      [&stdio_packet_builder, &packet_router, &socket_container](const vector<uint8_t>& received_data, AbstractSocket* socket) {
        shared_ptr<AbstractExtractor> extractor = socket->format()->create_extractor(received_data);

        shared_ptr<Packet::AbstractPacket> packet = stdio_packet_builder.build(extractor);
        if (packet == nullptr) {
          FAIL("Can't build packet in MGMT poller");
          return;
        }
        packet->prepare(packet_router);
        socket_container.send(packet);
      },
      on_error
  );

  // Test sending Ready packet
  shared_ptr<Packet::Control::Ready> ready_packet = make_shared<Packet::Control::Ready>();
  socket_container.send(ready_packet);
  REQUIRE(stdio_socket->get_num_write_buffers() == 1);

  // Verify that packet sent is correct
  shared_ptr<AbstractExtractor> fb_extractor = fb_format->create_extractor(stdio_socket->get_write_buffer(0));
  REQUIRE(fb_extractor->get_packet_code() == static_cast<uint16_t>(BaBLE::Payload::Ready));
  REQUIRE(fb_extractor->get_controller_id() == NON_CONTROLLER_ID);

  // Test receiving GetControllersList on StdIO
  shared_ptr<Packet::Meta::GetControllersList> get_controllers_list_packet = make_shared<Packet::Meta::GetControllersList>();
  stdio_socket->simulate_read(get_controllers_list_packet->to_bytes());
  REQUIRE(mgmt_socket->get_raw()->get_num_write_buffers() == 1);

  // Verify that GetControllersList command has been sent on MGMT socket
  shared_ptr<AbstractExtractor> mgmt_extractor = mgmt_format->create_extractor(mgmt_socket->get_raw()->get_write_buffer(0));
  REQUIRE(mgmt_extractor->get_packet_code() == Format::MGMT::CommandCode::GetControllersList);
  REQUIRE(mgmt_extractor->get_controller_id() == NON_CONTROLLER_ID);

  // Test receiving GetControllersList response on MGMT socket
  vector<uint8_t> mgmt_response_list{ 0x01, 0x00, 0xff, 0xff, 0x07, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 };

  mgmt_socket->simulate_read(mgmt_response_list);
  REQUIRE(stdio_socket->get_num_write_buffers() == 1);
  REQUIRE(mgmt_socket->get_raw()->get_num_write_buffers() == 2);

  // Verify that GetControllerInfo has been sent on MGMT socket (next step of the meta packet)
  shared_ptr<AbstractExtractor> mgmt_extractor2 = mgmt_format->create_extractor(mgmt_socket->get_raw()->get_write_buffer(1));
  REQUIRE(mgmt_extractor2->get_packet_code() == Format::MGMT::CommandCode::GetControllerInfo);
  REQUIRE(mgmt_extractor2->get_controller_id() == 0);

  // Test receiving GetControllersInfo response on MGMT socket
  vector<uint8_t> mgmt_response_info{ 0x01, 0x00, 0x00, 0x00, 0x1b, 0x01, 0x04, 0x00, 0x00, 0x30, 0xf3, 0xf6, 0x44, 0xe2, 0x48, 0x07,
                                      0x0f, 0x00, 0xff, 0xff, 0x00, 0x00, 0xd1, 0x0a, 0x00, 0x00, 0x0c, 0x01, 0x0c, 0x61, 0x72, 0x63,
                                      0x68, 0x2d, 0x6c, 0x61, 0x70, 0x74, 0x6f, 0x70, 0x2d, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00  };

  mgmt_socket->simulate_read(mgmt_response_info);
  REQUIRE(stdio_socket->get_num_write_buffers() == 2);
  REQUIRE(mgmt_socket->get_raw()->get_num_write_buffers() == 2);

  // Verify that GetControllerList final response has been sent on StdIO socket (last step of the meta packet)
  shared_ptr<AbstractExtractor> fb_extractor2 = fb_format->create_extractor(stdio_socket->get_write_buffer(1));
  REQUIRE(fb_extractor2->get_packet_code() == static_cast<uint16_t>(BaBLE::Payload::GetControllersList));
  REQUIRE(fb_extractor2->get_controller_id() == NON_CONTROLLER_ID);

  auto payload = static_pointer_cast<FlatbuffersFormatExtractor>(fb_extractor2)
      ->get_payload<const BaBLE::GetControllersList*>();
  REQUIRE(payload->controllers()->Length() == 1);
  REQUIRE(payload->controllers()->Get(0)->id() == 0);
  REQUIRE(payload->controllers()->Get(0)->name()->str() == "arch-laptop-1");
}

// Test a full use case sending Flatbuffers and HCI packets, and receiving Flatbuffers packets
TEST_CASE("Integration (with mocked socket) - HCI packet", "[integration][hci]") {
  string controller_address = "11:22:33:44:55:66";

  // Create loop
  uv_loop_t* loop = uv_default_loop();
  uv_loop_init(loop);

  // Formats
  shared_ptr<HCIFormat> hci_format = make_shared<HCIFormat>();
  shared_ptr<FlatbuffersFormat> fb_format = make_shared<FlatbuffersFormat>();

  // Mocked sockets
  shared_ptr<MockStdIOSocket> stdio_socket = make_shared<MockStdIOSocket>(loop, fb_format);
  shared_ptr<MockHCISocket> hci_socket = make_shared<MockHCISocket>(loop, hci_format, 0, Utils::extract_bd_address(controller_address));
  REQUIRE(hci_socket->get_raw()->is_open());
  REQUIRE(hci_socket->get_raw()->is_option_set());
  REQUIRE(hci_socket->get_raw()->is_binded());
  REQUIRE(hci_socket->get_raw()->is_connected() == false);
  REQUIRE(hci_socket->get_controller_id() == 0);
  REQUIRE(hci_socket->get_controller_address() == controller_address);

  // Socket container
  SocketContainer socket_container;
  socket_container
      .register_socket(stdio_socket)
      .register_socket(hci_socket);

  // PacketRouter
  shared_ptr<PacketRouter> packet_router = make_shared<PacketRouter>(loop);

  // PacketBuilder
  PacketBuilder hci_packet_builder(hci_format);
  hci_packet_builder
      .register_command<Packet::Commands::Central::ReadResponse>()
      .register_command<Packet::Commands::WriteResponse>();

  PacketBuilder stdio_packet_builder(fb_format);
  stdio_packet_builder
      .register_command<Packet::Commands::Central::ReadRequest>();

  auto on_error = [&stdio_socket, &socket_container](const Exceptions::BaBLEException& err) {
    CAPTURE(err);
    FAIL("Error received");
  };

  hci_socket->poll(
      [&hci_socket, &hci_packet_builder, &packet_router, &socket_container](const vector<uint8_t>& received_data, AbstractSocket* socket) {
        shared_ptr<AbstractExtractor> extractor = socket->format()->create_extractor(received_data);
        extractor->set_controller_id(hci_socket->get_controller_id());

        shared_ptr<Packet::AbstractPacket> packet = hci_packet_builder.build(extractor);
        if (packet == nullptr) {
          return;
        }
        packet = packet_router->route(packet_router, packet);
        packet->prepare(packet_router);
        socket_container.send(packet);
      },
      on_error
  );

  stdio_socket->poll(
      [&stdio_packet_builder, &packet_router, &socket_container](const vector<uint8_t>& received_data, AbstractSocket* socket) {
        shared_ptr<AbstractExtractor> extractor = socket->format()->create_extractor(received_data);

        shared_ptr<Packet::AbstractPacket> packet = stdio_packet_builder.build(extractor);
        if (packet == nullptr) {
          return;
        }
        packet->prepare(packet_router);
        socket_container.send(packet);
      },
      on_error
  );

  // Test send ReadRequest (HCI packet) and receive result
  shared_ptr<Packet::Commands::Central::ReadRequest> read_request_packet = make_shared<Packet::Commands::Central::ReadRequest>();
  REQUIRE_THROWS(read_request_packet->to_bytes());

  // Build a ReadRequest
  FlatbuffersFormatBuilder fb_builder(0, "test", "TEST");
  auto payload_request = BaBLE::CreateReadCentral(
      fb_builder,
      0x0040, // Connection handle
      0x0003  // Attribute handle
  );

  // Simulate a Flatbuffers packet received with a Read request inside. It will automatically send the matching HCI packet
  stdio_socket->simulate_read(fb_builder.build(payload_request, BaBLE::Payload::ReadCentral));
  REQUIRE(hci_socket->get_raw()->get_num_write_buffers() == 1);

  // Read the HCI packet sent (Read request)
  shared_ptr<AbstractExtractor> hci_extractor = hci_format->create_extractor(hci_socket->get_raw()->get_write_buffer(0));
  REQUIRE(hci_extractor->get_packet_code() == Format::HCI::AttributeCode::ReadRequest);

  // Simulate an HCI Read response
  vector<uint8_t> hci_response{0x02, 0x40, 0x20, 0x0a, 0x00, 0x06, 0x00, 0x04, 0x00, 0x0b, 0x62, 0x61, 0x42, 0x4C, 0x45};
  hci_socket->simulate_read(hci_response);
  REQUIRE(hci_socket->get_raw()->get_num_write_buffers() == 1);

  // Verify that the response has been correctly sent to StdIO
  REQUIRE(stdio_socket->get_num_write_buffers() == 1);

  // Verify that the Flatbuffers packet contains the response as expected
  shared_ptr<AbstractExtractor> fb_extractor = fb_format->create_extractor(stdio_socket->get_write_buffer(0));
  REQUIRE(fb_extractor->get_packet_code() == static_cast<uint16_t>(BaBLE::Payload::ReadCentral));
  REQUIRE(fb_extractor->get_controller_id() == 0);

  auto payload_response = static_pointer_cast<FlatbuffersFormatExtractor>(fb_extractor)->get_payload<const BaBLE::ReadCentral*>();
  REQUIRE(payload_response->connection_handle() == 0x0040);
  REQUIRE(payload_response->attribute_handle() == 0x0003);

  vector<uint8_t> read_bytes(payload_response->value()->begin(), payload_response->value()->end());
  string read_str = Utils::bytes_to_string(read_bytes);
  REQUIRE(read_str == "baBLE");
}
