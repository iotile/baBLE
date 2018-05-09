#ifndef BABLE_LINUX_FLATBUFFERSFORMATEXTRACTOR_HPP
#define BABLE_LINUX_FLATBUFFERSFORMATEXTRACTOR_HPP

#include <cstdint>
#include <Packet_generated.h>
#include <vector>

class FlatbuffersFormatExtractor {

public:
  explicit FlatbuffersFormatExtractor(const std::vector<uint8_t> & data) {
    m_data.assign(data.begin(), data.end());
    m_packet = Schemas::GetPacket(data.data());
  };

  template<typename T>
  T get_payload(Schemas::Payload payload_type) {
    auto payload = static_cast<T>(m_packet->payload());
    return payload;
  };

private:
  std::vector<uint8_t> m_data;
  const Schemas::Packet* m_packet;

};

#endif //BABLE_LINUX_FLATBUFFERSFORMATEXTRACTOR_HPP
