#include <catch.hpp>
#include <uv.h>
#include "utils/string_formats.hpp"
#include "Exceptions/BaBLEException.hpp"

using namespace std;

TEST_CASE("Format bluetooth device address to string", "[unit][utils][string]") {
  array<uint8_t, 6> bd_address_array{0x63, 0xc0, 0x76, 0xf4, 0xfb, 0xe7};
  REQUIRE(Utils::format_bd_address(bd_address_array) == "e7:fb:f4:76:c0:63");
}

TEST_CASE("Format uuid to string", "[unit][utils][string]") {
  vector<uint8_t> uuid_vector{0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
  REQUIRE(Utils::format_uuid(uuid_vector) == "aabbccddee");
}

TEST_CASE("Format bytes array to string", "[unit][utils][string]") {
  vector<uint8_t> bytes_vector{0x01, 0x02, 0x0F, 0x0A, 0xFF};
  REQUIRE(Utils::format_bytes_array(bytes_vector) == "[0x01, 0x02, 0x0f, 0x0a, 0xff]");

  array<uint8_t, 4> bytes_array{0x02, 0x05, 0xBA, 0xFC};
  REQUIRE(Utils::format_bytes_array(bytes_array) == "[0x02, 0x05, 0xba, 0xfc]");
}

TEST_CASE("Format bytes to string", "[unit][utils][string]") {
  vector<uint8_t> bytes_vector{0x62, 0x61, 0x42, 0x4C, 0x45};
  REQUIRE(Utils::bytes_to_string(bytes_vector) == "baBLE");

  array<uint8_t, 5> bytes_array{0x62, 0x61, 0x42, 0x4C, 0x45};
  REQUIRE(Utils::bytes_to_string(bytes_array) == "baBLE");

  vector<uint16_t> empty_vector{};
  REQUIRE(Utils::bytes_to_string(empty_vector).empty());
}

TEST_CASE("Format string to number", "[unit][utils][string]") {
  string num1 = "4783";
  REQUIRE(Utils::string_to_number<uint16_t>(num1) == 4783);

  string num2 = "4783.5";
  REQUIRE(Utils::string_to_number<uint16_t>(num2) == 4783);

  string num3 = "4783.5";
  REQUIRE(Utils::string_to_number<float>(num3) == 4783.0);

  string num4 = "test";
  REQUIRE_THROWS_AS(Utils::string_to_number<uint16_t>(num4), Exceptions::BaBLEException);

  string num5 = "0x12AF";
  REQUIRE(Utils::string_to_number<uint16_t>(num5, 16) == 0x12AF);
}

TEST_CASE("Extract bluetooth device address from string", "[unit][utils][string]") {
  string bd_address_str = "e7:fb:f4:76:c0:63";
  REQUIRE(Utils::extract_bd_address(bd_address_str) == array<uint8_t, 6>{0x63, 0xc0, 0x76, 0xf4, 0xfb, 0xe7});

  string bd_address_str2 = "E7:FB:F4:76:C0:63";
  REQUIRE(Utils::extract_bd_address(bd_address_str2) == array<uint8_t, 6>{0x63, 0xc0, 0x76, 0xf4, 0xfb, 0xe7});

  string bd_address_str3 = "test";
  REQUIRE_THROWS_AS(Utils::extract_bd_address(bd_address_str3), Exceptions::BaBLEException);

  string bd_address_str4 = "1025:5646:3213:13243";
  REQUIRE_THROWS_AS(Utils::extract_bd_address(bd_address_str4), Exceptions::BaBLEException);
}
