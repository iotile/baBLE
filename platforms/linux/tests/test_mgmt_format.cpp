#include <catch.hpp>
#include <uv.h>
#include "Format/MGMT/MGMTFormat.hpp"

using namespace std;

TEST_CASE("MGMTFormat", "[unit][format][mgmt]") {
  MGMTFormat mgmt_format;

  REQUIRE(mgmt_format.get_packet_type() == Packet::Type::MGMT);

  Format::MGMT::EventCode type_code;
  type_code = Format::MGMT::EventCode::CommandComplete;
  REQUIRE(mgmt_format.get_header_length(type_code) == Format::MGMT::header_length);
  REQUIRE(mgmt_format.is_command(type_code) == true);
  REQUIRE(mgmt_format.is_event(type_code) == false);

  type_code = Format::MGMT::EventCode::CommandStatus;
  REQUIRE(mgmt_format.get_header_length(type_code) == Format::MGMT::header_length);
  REQUIRE(mgmt_format.is_command(type_code) == true);
  REQUIRE(mgmt_format.is_event(type_code) == false);

  type_code = Format::MGMT::EventCode::IndexAdded;
  REQUIRE(mgmt_format.get_header_length(type_code) == Format::MGMT::header_length);
  REQUIRE(mgmt_format.is_command(type_code) == false);
  REQUIRE(mgmt_format.is_event(type_code) == true);
}

TEST_CASE("MGMTFormatExtractor with event packet", "[unit][format][mgmt]") {
  /* Raw bytes represents this :
      MGMT Index Added event
       Controller ID: 1
  */
  vector<uint8_t> received_data = { 0x04, 0x00, 0x01, 0x00, 0x00, 0x00 };

  MGMTFormat mgmt_format;
  shared_ptr<MGMTFormatExtractor> mgmt_extractor = static_pointer_cast<MGMTFormatExtractor>(
      mgmt_format.create_extractor(received_data)
  );

  REQUIRE(mgmt_extractor != nullptr);
  REQUIRE(mgmt_extractor->extract_payload_length(received_data) == 0);
  REQUIRE(mgmt_extractor->get_packet_code() == Format::MGMT::EventCode::IndexAdded);
  REQUIRE(mgmt_extractor->get_controller_id() == 1);
}

TEST_CASE("MGMTFormatExtractor with command packet", "[unit][format][mgmt]") {
  /* Raw bytes represents this :
       MGMT Command Complete Response
       Controller ID: 0
       SetPowered command
         Status: Success (0x00)
         Current settings: [00 00 0A D1] (meaning Powered + other settings)
  */
  vector<uint8_t> received_data = { 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x05, 0x00, 0x00, 0xd1, 0x0a, 0x00, 0x00 };

  MGMTFormat mgmt_format;
  shared_ptr<MGMTFormatExtractor> mgmt_extractor = static_pointer_cast<MGMTFormatExtractor>(
      mgmt_format.create_extractor(received_data)
  );

  REQUIRE(mgmt_extractor != nullptr);
  REQUIRE(mgmt_extractor->extract_payload_length(received_data) == 7);
}

TEST_CASE("MGMTFormatBuilder", "[unit][format][mgmt]") {
  MGMTFormatBuilder mgmt_builder(0);
  mgmt_builder
      .set_opcode(Format::MGMT::CommandCode::SetPowered)
      .add<bool>(true);  // Powered

  vector<uint8_t> result = mgmt_builder.build();
  REQUIRE(result == vector<uint8_t>{0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01});
}
