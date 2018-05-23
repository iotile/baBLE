#ifndef BABLE_LINUX_FLATBUFFERSFORMATEXTRACTOR_HPP
#define BABLE_LINUX_FLATBUFFERSFORMATEXTRACTOR_HPP

#include <cstdint>
#include <Packet_generated.h>
#include <vector>
#include "../AbstractExtractor.hpp"
#include "../../Exceptions/WrongFormat/WrongFormatException.hpp"

class FlatbuffersFormatExtractor : public AbstractExtractor {

public:
  static uint16_t extract_type_code(const std::vector<uint8_t>& data);
  static uint16_t extract_packet_code(const std::vector<uint8_t>& data);
  static uint16_t extract_controller_id(const std::vector<uint8_t>& data);
  static uint16_t extract_payload_length(const std::vector<uint8_t>& data);

  static void verify(const std::vector<uint8_t>& data);

  // Constructors
  explicit FlatbuffersFormatExtractor(const std::vector<uint8_t> & data);

  // Getters
  template<typename T>
  T get_payload() const;

  inline uint16_t get_controller_id() const override {
    return m_packet->controller_id();
  };

  inline std::string get_uuid_request() const {
    return m_packet->uuid()->str();
  };

private:
  const BaBLE::Packet* m_packet;

};

template<typename T>
T FlatbuffersFormatExtractor::get_payload() const {
  auto payload = static_cast<T>(m_packet->payload());
  return payload;
};

#endif //BABLE_LINUX_FLATBUFFERSFORMATEXTRACTOR_HPP
