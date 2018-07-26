#ifndef BABLE_FLATBUFFERSFORMATEXTRACTOR_HPP
#define BABLE_FLATBUFFERSFORMATEXTRACTOR_HPP

#include <Packet_generated.h>
#include "Format/AbstractExtractor.hpp"

class FlatbuffersFormatExtractor : public AbstractExtractor {

public:
  static uint16_t extract_payload_length(const std::vector<uint8_t>& data);

  static void verify(const std::vector<uint8_t>& data);

  // Constructors
  explicit FlatbuffersFormatExtractor(const std::vector<uint8_t> & data);

  // Getters
  template<typename T>
  T get_payload() const;

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

#endif //BABLE_FLATBUFFERSFORMATEXTRACTOR_HPP
