#include "ReadByTypeResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    ReadByTypeResponse::ReadByTypeResponse(Packet::Type initial_type, Packet::Type translated_type)
        : ResponsePacket(initial_type, translated_type) {
      m_id = Packet::Id::ReadByTypeResponse;
      m_last_ending_handle = 0;
    }

    void ReadByTypeResponse::unserialize(HCIFormatExtractor& extractor) {
      uint16_t data_length = extractor.get_data_length();

      if (data_length <= 0) {
        throw Exceptions::WrongFormatException("Receive HCI 'ReadByTypeResponse' with no data.", m_uuid_request);
      }

      auto attribute_length = extractor.get_value<uint8_t>();
      data_length -= 1; // To remove attribute_length from remaining data_length

      size_t num_characteristics = data_length / attribute_length;
      m_characteristics.clear();
      m_characteristics.reserve(num_characteristics);

      while (data_length >= attribute_length) {
        Format::HCI::Characteristic characteristic{};
        characteristic.handle = extractor.get_value<uint16_t>();
        characteristic.properties = extractor.get_value<uint8_t>();
        characteristic.value_handle = extractor.get_value<uint16_t>();
        characteristic.uuid = extractor.get_vector<uint8_t>(attribute_length
                                                                - sizeof(characteristic.handle)
                                                                - sizeof(characteristic.properties)
                                                                - sizeof(characteristic.value_handle));

        m_characteristics.push_back(characteristic);
        m_last_ending_handle = characteristic.value_handle;
        data_length -= attribute_length;
      }
    }

    vector<uint8_t> ReadByTypeResponse::serialize(AsciiFormatBuilder& builder) const {
      ResponsePacket::serialize(builder);
      builder
          .set_name("ReadByType")
          .add("Last ending handle", m_last_ending_handle);

      for (auto& characteristic : m_characteristics) {
        builder
            .add("Handle", characteristic.handle)
            .add("Properties", characteristic.properties)
            .add("Value handle", characteristic.value_handle)
            .add("UUID", characteristic.uuid);
      }

      return builder.build();
    }

  }

}