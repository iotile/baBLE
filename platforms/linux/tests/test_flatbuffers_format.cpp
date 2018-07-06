#include <catch.hpp>
#include <uv.h>
#include "Format/Flatbuffers/FlatbuffersFormat.hpp"

using namespace std;

TEST_CASE("FlatbuffersFormat", "[unit][format][flatbuffers]") {
  FlatbuffersFormat fb_format;

  REQUIRE(fb_format.get_packet_type() == Packet::Type::FLATBUFFERS);
  REQUIRE(fb_format.is_command(0) == true);
  REQUIRE(fb_format.is_event(0) == false);
  REQUIRE(fb_format.get_header_length(0) == 0);
}

TEST_CASE("FlatbuffersFormatExtractor with command packet", "[unit][format][flatbuffers]") {
  /* Raw bytes represents this :
       CancelConnection BaBLE command
         UUID: "0002"
         Controller ID: 0
  */
  vector<uint8_t> received_data = { 0x10, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x10, 0x00, 0x0c, 0x00, 0x09, 0x00, 0x04, 0x00,
                                    0x0a, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
                                    0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x32, 0x00, 0x00,
                                    0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00 };

  REQUIRE_NOTHROW(FlatbuffersFormatExtractor::verify(received_data));

  FlatbuffersFormat fb_format;
  shared_ptr<FlatbuffersFormatExtractor> fb_extractor = static_pointer_cast<FlatbuffersFormatExtractor>(
      fb_format.create_extractor(received_data)
  );

  REQUIRE(fb_extractor != nullptr);
  REQUIRE_THROWS(fb_extractor->extract_payload_length(received_data));
  REQUIRE(fb_extractor->get_controller_id() == 0);
  REQUIRE(fb_extractor->get_uuid_request() == "0002");
  REQUIRE(fb_extractor->get_packet_code() == static_cast<uint16_t>(BaBLE::Payload::CancelConnection));
}

TEST_CASE("FlatbuffersFormatBuilder", "[unit][format][flatbuffers]") {
  FlatbuffersFormatBuilder fb_builder(0, "0001", "TEST");
  fb_builder.set_status(BaBLE::StatusCode::Success, 0);

  auto payload = BaBLE::CreateCancelConnection(fb_builder);
  vector<uint8_t> result = fb_builder.build(payload, BaBLE::Payload::CancelConnection);

  REQUIRE(result == vector<uint8_t>{0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x14, 0x00, 0x04, 0x00, 0x11, 0x00,
                                    0x0c, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x12, 0x00, 0x00, 0x00,
                                    0x10, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x01,
                                    0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00,
                                    0x04, 0x00, 0x00, 0x00, 0x54, 0x45, 0x53, 0x54, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
                                    0x04, 0x00, 0x04, 0x00, 0x00, 0x00});
}
