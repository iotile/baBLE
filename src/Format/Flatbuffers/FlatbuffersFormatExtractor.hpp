#ifndef BABLE_LINUX_FLATBUFFERSFORMATEXTRACTOR_HPP
#define BABLE_LINUX_FLATBUFFERSFORMATEXTRACTOR_HPP

#include <cstdint>
#include <Packet_generated.h>
#include <vector>
#include "../../Exceptions/WrongFormat/WrongFormatException.hpp"

class FlatbuffersFormatExtractor {

public:
  static uint16_t extract_payload_type(const std::vector<uint8_t>& data);
  static uint16_t extract_controller_id(const std::vector<uint8_t>& data);
  static void verify(const std::vector<uint8_t>& data);

  // Constructors
  explicit FlatbuffersFormatExtractor(const std::vector<uint8_t> & data);

  // Getters
  template<typename T>
  T get_payload(Schemas::Payload payload_type) const;
  uint16_t get_controller_id() const;

private:
  std::vector<uint8_t> m_data;
  const Schemas::Packet* m_packet;

};

template<typename T>
T FlatbuffersFormatExtractor::get_payload(Schemas::Payload payload_type) const {
  auto payload = static_cast<T>(m_packet->payload());
  return payload;
};

#endif //BABLE_LINUX_FLATBUFFERSFORMATEXTRACTOR_HPP
