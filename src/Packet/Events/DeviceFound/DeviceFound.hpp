#ifndef BABLE_LINUX_DEVICEFOUND_HPP
#define BABLE_LINUX_DEVICEFOUND_HPP

#include "../EventPacket.hpp"
#include "../../../Format/Ascii/AsciiFormat.hpp"

namespace Packet::Events {

  class DeviceFound : public EventPacket<DeviceFound> {

  public:
    static const uint16_t event_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::EventCode::DeviceFound;

        case Packet::Type::ASCII:
          return Format::Ascii::EventCode::DeviceFound;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::DeviceFound);

        default:
          throw std::runtime_error("Current type has no known id (DeviceFound).");
      }
    };

    DeviceFound(Packet::Type initial_type, Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_address_type = 0;
      m_rssi = 0;
      m_eir_data_length = 0;
    };

    void import(MGMTFormatExtractor& extractor) override {
      EventPacket::import(extractor);
      m_address = extractor.get_array<uint8_t, 6>();
      m_address_type = extractor.get_value<uint8_t>();
      m_rssi = extractor.get_value<int8_t>();
      m_flags = extractor.get_array<uint8_t, 4>();
      m_eir_data_length = extractor.get_value<uint16_t>();

      if (m_eir_data_length > 0) {
        m_eir_data = extractor.get_vector<uint8_t>(m_eir_data_length);

        try {
          m_eir = extractor.parse_eir(m_eir_data);

        } catch (std::invalid_argument& err) {
          LOG.error("Can't parse EIR", "DeviceFound");
          LOG.debug(m_eir_data, "DeviceFound");
        }
      }
    };

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override {
      EventPacket::serialize(builder);

      builder
          .set_name("Discovering")
          .add("Address", AsciiFormat::format_bd_address(m_address))
          .add("Address type", m_address_type)
          .add("RSSI", m_rssi)
          .add("Flags", m_flags)
          .add("EIR data length: ", m_eir_data_length)
          .add("EIR flags: ", m_eir.flags)
          .add("EIR UUID: ", AsciiFormat::format_uuid(m_eir.uuid))
          .add("EIR company ID: ", m_eir.company_id)
          .add("EIR device name", AsciiFormat::format_device_name(m_eir.device_name));

      return builder.build();
    };

    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override {
      EventPacket::serialize(builder);

      std::vector<uint8_t> flags_vector(m_flags.begin(), m_flags.end());

      auto address = builder.CreateString(AsciiFormat::format_bd_address(m_address));
      auto flags = builder.CreateVector(flags_vector);
      auto uuid = builder.CreateString(AsciiFormat::format_uuid(m_eir.uuid));
      auto device_name = builder.CreateString(AsciiFormat::format_device_name(m_eir.device_name));

      auto payload = Schemas::CreateDeviceFound(
          builder,
          m_controller_id,
          address,
          m_address_type,
          m_rssi,
          flags,
          uuid,
          m_eir.company_id,
          device_name
      );

      return builder.build(payload, Schemas::Payload::DeviceFound);
    }

  private:
    std::array<uint8_t, 6> m_address{};
    uint8_t m_address_type;
    int8_t m_rssi;
    std::array<uint8_t, 4> m_flags{};
    uint16_t m_eir_data_length;
    std::vector<uint8_t> m_eir_data;
    EIR m_eir{};
  };

}

#endif //BABLE_LINUX_DEVICEFOUND_HPP
