#include <catch.hpp>
#include <uv.h>
#include "Format/HCI/HCIFormat.hpp"
#include "Exceptions/BaBLEException.hpp"

using namespace std;

TEST_CASE("HCIFormat", "[unit][format][hci]") {
  HCIFormat hci_format;

  REQUIRE(hci_format.get_packet_type() == Packet::Type::HCI);

  Format::HCI::Type type_code;
  // Test a command packet
  type_code = Format::HCI::Type::Command;
  REQUIRE(hci_format.get_header_length(type_code) == Format::HCI::command_header_length);
  REQUIRE(hci_format.is_command(type_code) == true);
  REQUIRE(hci_format.is_event(type_code) == false);

  // Test an event packet
  type_code = Format::HCI::Type::Event;
  REQUIRE(hci_format.get_header_length(type_code) == Format::HCI::event_header_length);
  REQUIRE(hci_format.is_command(type_code) == false);
  REQUIRE(hci_format.is_event(type_code) == true);

  // Test an asynchronous data packet
  type_code = Format::HCI::Type::AsyncData;
  REQUIRE(hci_format.get_header_length(type_code) == Format::HCI::async_data_header_length);
  REQUIRE(hci_format.is_command(type_code) == true);
  REQUIRE(hci_format.is_event(type_code) == false);

  // Test a synchronous data packet
  type_code = Format::HCI::Type::SyncData;
  REQUIRE_THROWS_AS(hci_format.get_header_length(type_code), Exceptions::BaBLEException);
  REQUIRE(hci_format.is_command(type_code) == false);
  REQUIRE(hci_format.is_event(type_code) == false);
}

TEST_CASE("HCIFormatExtractor with event packet", "[unit][format][hci]") {
  /* Raw bytes representing this HCI packet:
      HCI Packet Type: HCI Event (0x04)
      Bluetooth HCI Event - LE Meta
        Event Code: LE Meta (0x3e)
        Parameter Total Length: 42
        Sub Event: LE Advertising Report (0x02)
        Num Reports: 1
        Event Type: Scan Response (0x04)
        Peer Address Type: Random Device Address (0x01)
        BD_ADDR: e7:fb:f4:76:c0:63 (e7:fb:f4:76:c0:63)
        Data Length: 30
        Advertising Data
          Manufacturer Specific
            Length: 19
            Type: Manufacturer Specific (0xff)
            Company ID: Arch Systems Inc. (0x03c0)
            Data: a1031c10510000005100000050000000
          Device Name: baBLE
            Length: 6
            Type: Device Name (0x09)
            Device Name: baBLE
          Unused
        RSSI (dB): -64
  */
  vector<uint8_t> received_data = { 0x04, 0x3e, 0x2a, 0x02, 0x01, 0x04, 0x01, 0x63, 0xc0, 0x76, 0xf4, 0xfb, 0xe7, 0x1e,
                                    0x13, 0xff, 0xc0, 0x03, 0xa1, 0x03, 0x1c, 0x10, 0x51, 0x00, 0x00, 0x00, 0x51, 0x00,
                                    0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x06, 0x09, 0x62, 0x61, 0x42, 0x4C, 0x45, 0x00,
                                    0x00, 0x00, 0xc0 };

  HCIFormat hci_format;
  shared_ptr<HCIFormatExtractor> hci_extractor = static_pointer_cast<HCIFormatExtractor>(
      hci_format.create_extractor(received_data)
  );

  // Test extracting information from header
  REQUIRE(hci_extractor != nullptr);
  REQUIRE(hci_extractor->extract_type_code(received_data) == Format::HCI::Type::Event);
  REQUIRE(hci_extractor->extract_payload_length(received_data) == 42);

  // Test extracting data from payload
  auto num_reports = hci_extractor->get_value<uint8_t>();
  auto type = static_cast<Format::HCI::AdvertisingReportType>(hci_extractor->get_value<uint8_t>());
  REQUIRE(type == Format::HCI::AdvertisingReportType::ScanResponse);

  auto address_type = hci_extractor->get_value<uint8_t>();
  REQUIRE(address_type == 0x01);

  auto address = hci_extractor->get_array<uint8_t, 6>();
  REQUIRE(address == array<uint8_t, 6>{0x63, 0xc0, 0x76, 0xf4, 0xfb, 0xe7}); // It is normal that the address is reversed (due to HCI using little endian)

  auto eir_data_length = hci_extractor->get_value<uint8_t>();
  REQUIRE(eir_data_length == 30);

  auto eir_data = hci_extractor->get_vector<uint8_t>(eir_data_length);
  REQUIRE(eir_data == vector<uint8_t>{0x13, 0xff, 0xc0, 0x03, 0xa1, 0x03, 0x1c, 0x10, 0x51, 0x00, 0x00, 0x00, 0x51, 0x00,
                                      0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x06, 0x09, 0x62, 0x61, 0x42, 0x4C, 0x45, 0x00,
                                      0x00, 0x00});

  // Test parsing the Extended Inquiry Response (EIR)
  Format::HCI::EIR eir = hci_extractor->parse_eir(eir_data);
  REQUIRE(eir.device_name == vector<uint8_t>{0x62, 0x61, 0x42, 0x4C, 0x45});
  REQUIRE(eir.company_id == 0x03C0);
  REQUIRE(eir.manufacturer_data == vector<uint8_t>{0xa1, 0x03, 0x1c, 0x10, 0x51, 0x00, 0x00, 0x00, 0x51, 0x00, 0x00,
                                                   0x00, 0x50, 0x00, 0x00, 0x00});

  auto rssi = hci_extractor->get_value<int8_t>();
  REQUIRE(rssi == -64);

  // Test extracting more data than the packet contains: should throw an error
  REQUIRE_THROWS_AS(hci_extractor->get_value<uint8_t>(), Exceptions::BaBLEException);
}

TEST_CASE("HCIFormatExtractor with command packet", "[unit][format][hci]") {
  // We do this with a command packet because it has a different header from the event packets

  /* Raw bytes representing this HCI packet:
       HCI Packet Type: HCI Command (0x01)
       Bluetooth HCI Command - LE Set Scan Parameters
         Command Opcode: LE Set Scan Parameters (0x200b)
         Parameter Total Length: 7
         Scan Type: Active (0x01)
         Scan Interval: 16 (10 msec)
         Scan Window: 16 (10 msec)
         Own Address Type: Public Device Address (0x00)
         Scan Filter Policy: Accept all advertisements. Ignore directed advertisements not addressed to this device (0x00)
  */
  vector<uint8_t> received_data = { 0x01, 0x0b, 0x20, 0x07, 0x01, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00 };

  HCIFormat hci_format;
  shared_ptr<HCIFormatExtractor> hci_extractor = static_pointer_cast<HCIFormatExtractor>(
      hci_format.create_extractor(received_data)
  );

  REQUIRE(hci_extractor != nullptr);
  REQUIRE(hci_extractor->extract_type_code(received_data) == Format::HCI::Type::Command);
  REQUIRE(hci_extractor->extract_payload_length(received_data) == 7);
}

TEST_CASE("HCIFormatBuilder", "[unit][format][hci]") {
  HCIFormatBuilder hci_builder(0);
  hci_builder
      .set_opcode(Format::HCI::CommandCode::SetScanParameters)  // Opcode
      .add<uint8_t>(0x01)  // Scan type
      .add<uint16_t>(16)   // Scan interval
      .add<uint16_t>(16)   // Scan window
      .add<uint8_t>(0x00)  // Address type
      .add<uint8_t>(0x00); // Scan filter policy

  vector<uint8_t> result = hci_builder.build(Format::HCI::Type::Command);
  REQUIRE(result == vector<uint8_t>{0x01, 0x0b, 0x20, 0x07, 0x01, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00});

  // Build it with a different type to see if the header has correctly changed
  result = hci_builder.build(Format::HCI::Type::AsyncData);
  REQUIRE(result.at(0) == 0x02);

  // We can't build event packets because they are only sent by the controller
  REQUIRE_THROWS(hci_builder.build(Format::HCI::Type::Event));
}
