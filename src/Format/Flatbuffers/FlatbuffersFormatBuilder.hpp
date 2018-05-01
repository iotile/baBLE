#ifndef BABLE_LINUX_FLATBUFFERSFORMATBUILDER_HPP
#define BABLE_LINUX_FLATBUFFERSFORMATBUILDER_HPP

#include <flatbuffers/flatbuffers.h>
#include <Packet_generated.h>

class FlatbuffersFormatBuilder : public flatbuffers::FlatBufferBuilder {

public:
  using flatbuffers::FlatBufferBuilder::FlatBufferBuilder;

  template<class T>
  std::vector<uint8_t> build(const flatbuffers::Offset<T>& payload, Schemas::Payload payload_type) {
    Schemas::PacketBuilder packet_builder(*this);
    packet_builder.add_payload_type(payload_type);
    packet_builder.add_payload(payload.Union());
    auto packet = packet_builder.Finish();
    Finish(packet);

    uint8_t* buffer = GetBufferPointer();
    size_t buffer_size = GetSize();

    std::vector<uint8_t> result(buffer, buffer + buffer_size);
    return result;
  }

};

#endif //BABLE_LINUX_FLATBUFFERSFORMATBUILDER_HPP
