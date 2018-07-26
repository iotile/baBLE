#ifndef BABLE_FLATBUFFERSFORMATBUILDER_HPP
#define BABLE_FLATBUFFERSFORMATBUILDER_HPP

#include <Packet_generated.h>

class FlatbuffersFormatBuilder : public flatbuffers::FlatBufferBuilder {

public:
  explicit FlatbuffersFormatBuilder(uint16_t controller_id, const std::string& uuid_request, const std::string& native_class);

  FlatbuffersFormatBuilder& set_status(BaBLE::StatusCode status, uint8_t native_status = 0x00);

  template<class T>
  std::vector<uint8_t> build(const flatbuffers::Offset<T>& payload, BaBLE::Payload payload_type);

private:
  uint16_t m_controller_id;
  std::string m_native_class;
  std::string m_uuid_request;
  BaBLE::StatusCode m_status;
  uint8_t m_native_status;

};

template<class T>
std::vector<uint8_t> FlatbuffersFormatBuilder::build(const flatbuffers::Offset<T>& payload, BaBLE::Payload payload_type) {

  auto native_class_offset = CreateString(m_native_class);
  auto uuid_request_offset = CreateString(m_uuid_request);

  BaBLE::PacketBuilder packet_builder(*this);
  packet_builder.add_controller_id(m_controller_id);
  packet_builder.add_payload_type(payload_type);
  packet_builder.add_payload(payload.Union());
  packet_builder.add_native_class(native_class_offset);
  packet_builder.add_status(m_status);
  packet_builder.add_native_status(m_native_status);
  packet_builder.add_uuid(uuid_request_offset);
  auto packet = packet_builder.Finish();
  Finish(packet);

  uint8_t* buffer = GetBufferPointer();
  size_t buffer_size = GetSize();

  std::vector<uint8_t> result(buffer, buffer + buffer_size);
  return result;
};

#endif //BABLE_FLATBUFFERSFORMATBUILDER_HPP
