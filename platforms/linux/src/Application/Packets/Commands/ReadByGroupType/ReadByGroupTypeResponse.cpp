#include "ReadByGroupTypeResponse.hpp"

using namespace std;

namespace Packet::Commands {

  ReadByGroupTypeResponse::ReadByGroupTypeResponse(Packet::Type initial_type, Packet::Type translated_type)
      : ResponsePacket(initial_type, translated_type) {
    m_id = Packet::Id::ReadByGroupTypeResponse;
    m_last_group_end_handle = 0;
  }

  void ReadByGroupTypeResponse::unserialize(HCIFormatExtractor& extractor) {
    uint16_t data_length = extractor.get_data_length();

    if (data_length <= 0) {
      throw Exceptions::WrongFormatException("Receive HCI 'ReadByGroupTypeResponse' with no data.", m_uuid_request);
    }

    auto attribute_length = extractor.get_value<uint8_t>();
    data_length -= 1; // To remove attribute_length from remaining data_length

    size_t num_services = data_length / attribute_length;
    m_services.clear();
    m_services.reserve(num_services);

    while(data_length >= attribute_length) {
      Format::HCI::Service service{};
      service.handle = extractor.get_value<uint16_t>();
      service.group_end_handle = extractor.get_value<uint16_t>();
      service.uuid = extractor.get_vector<uint8_t>(attribute_length
                                                      - sizeof(service.handle)
                                                      - sizeof(service.group_end_handle));

      m_services.push_back(service);
      m_last_group_end_handle = service.group_end_handle;
      data_length -= attribute_length;
    }
  };

  vector<uint8_t> ReadByGroupTypeResponse::serialize(AsciiFormatBuilder& builder) const {
    ResponsePacket::serialize(builder);
    builder
        .set_name("ReadByGroupType")
        .add("Last group end handle", m_last_group_end_handle);

    for (auto& service : m_services) {
      builder
          .add("Handle", service.handle)
          .add("Group end handle", service.group_end_handle)
          .add("UUID", service.uuid);
    }

    return builder.build();
  };

}