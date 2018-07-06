#include <catch.hpp>
#include <uv.h>
#include "Application/Packets/PacketUuid.hpp"
#include "Format/HCI/constants.hpp"

using namespace std;

TEST_CASE("PacketUuid should match", "[unit][packetuuid]") {
  Packet::PacketUuid request_uuid(
      Packet::Type::HCI,  // Packet type
      0, // Controller ID
      0x0040,  // Connection handle
      Format::HCI::AttributeCode::ReadByTypeRequest  // Expected response packet code
  );

  Packet::PacketUuid response_uuid(
      Packet::Type::HCI,  // Packet type
      0, // Controller ID
      0x0040,  // Connection handle
      Format::HCI::AttributeCode::ReadByTypeRequest,  // Expected response packet code
      Format::HCI::AttributeCode::ReadByTypeRequest  // Request packet code
  );

  Packet::PacketUuid response_uuid2(
      Packet::Type::HCI,  // Packet type
      0, // Controller ID
      0x0040,  // Connection handle
      Format::HCI::AttributeCode::ReadByTypeResponse,  // Expected response packet code
      Format::HCI::AttributeCode::ReadByTypeResponse  // Request packet code
  );

  // Packets have exactly the same uuid
  REQUIRE(response_uuid.match(request_uuid) == true);
  REQUIRE((response_uuid == request_uuid) == true);

  // Response received is a response packet for the request (but the response doesn't know the request)
  request_uuid.response_packet_code = Format::HCI::AttributeCode::ReadByTypeResponse;
  REQUIRE(response_uuid2.match(request_uuid) == true);
  REQUIRE((response_uuid2 == request_uuid) == false);

  // The request doesn't know the connection handle (so match all)
  request_uuid.connection_handle = 0;
  REQUIRE(response_uuid2.match(request_uuid) == true);
  REQUIRE((response_uuid2 == request_uuid) == false);
}

TEST_CASE("PacketUuid should not match", "[unit][packetuuid]") {
  Packet::PacketUuid request_uuid(
      Packet::Type::HCI,  // Packet type
      0, // Controller ID
      0x0040,  // Connection handle
      Format::HCI::AttributeCode::ReadByTypeRequest  // Expected response packet code
  );

  Packet::PacketUuid response_uuid(
      Packet::Type::HCI,  // Packet type
      0, // Controller ID
      0x0040,  // Connection handle
      Format::HCI::AttributeCode::ReadByTypeRequest,  // Expected response packet code
      Format::HCI::AttributeCode::ReadByTypeRequest  // Request packet code
  );

  // Different packet type
  Packet::PacketUuid different_packet_type = response_uuid;
  different_packet_type.packet_type = Packet::Type::MGMT;
  REQUIRE(different_packet_type.match(request_uuid) == false);
  REQUIRE((different_packet_type == request_uuid) == false);

  // Different controller id
  Packet::PacketUuid different_controller_id = response_uuid;
  different_controller_id.controller_id = 1;
  REQUIRE(different_controller_id.match(request_uuid) == false);
  REQUIRE((different_controller_id == request_uuid) == false);

  // Different connection handle
  Packet::PacketUuid different_connection_handle = response_uuid;
  different_connection_handle.connection_handle = 0x0041;
  REQUIRE(different_connection_handle.match(request_uuid) == false);
  REQUIRE((different_connection_handle == request_uuid) == false);

  // Different response packet code
  Packet::PacketUuid different_response_packet_code = response_uuid;
  different_response_packet_code.response_packet_code = Format::HCI::AttributeCode::ReadByTypeResponse;
  REQUIRE(different_response_packet_code.match(request_uuid) == false);
  REQUIRE((different_response_packet_code == request_uuid) == false);

  // Unexpected response
  Packet::PacketUuid unexpected_response = request_uuid;
  unexpected_response.request_packet_code = Format::HCI::AttributeCode::ReadByGroupTypeRequest;
  REQUIRE(unexpected_response.match(request_uuid) == false);
  REQUIRE((unexpected_response == request_uuid) == false);

  // Wrong way
  Packet::PacketUuid request = request_uuid;
  request.response_packet_code = Format::HCI::AttributeCode::ReadByTypeResponse;
  Packet::PacketUuid response = request_uuid;
  response.response_packet_code = Format::HCI::AttributeCode::ReadByTypeResponse;
  response.request_packet_code = Format::HCI::AttributeCode::ReadByTypeResponse;
  REQUIRE(response.match(request) == true);
  REQUIRE(request.match(response) == false);
}
