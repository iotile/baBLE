#ifndef BABLE_LINUX_FLATBUFFERSFORMATBUILDER_HPP
#define BABLE_LINUX_FLATBUFFERSFORMATBUILDER_HPP

#include <Packet_generated.h>

class FlatbuffersFormatBuilder : public flatbuffers::FlatBufferBuilder {

public:
  using flatbuffers::FlatBufferBuilder::FlatBufferBuilder;

  template<class T>
  std::vector<uint8_t> build(const flatbuffers::Offset<T>& payload, Schemas::Payload payload_type,
                             const std::string& native_class,
                             Schemas::StatusCode status = Schemas::StatusCode::Success, uint8_t native_status = 0x00) {
    auto native_class_offset = CreateString(native_class);

    Schemas::PacketBuilder packet_builder(*this);
    packet_builder.add_payload_type(payload_type);
    packet_builder.add_payload(payload.Union());
    packet_builder.add_status(status);
    packet_builder.add_native_status(native_status);
    packet_builder.add_native_class(native_class_offset);
    auto packet = packet_builder.Finish();
    Finish(packet);

    uint8_t* buffer = GetBufferPointer();
    size_t buffer_size = GetSize();

    std::vector<uint8_t> result(buffer, buffer + buffer_size);
    return result;
  }

};

#endif //BABLE_LINUX_FLATBUFFERSFORMATBUILDER_HPP
