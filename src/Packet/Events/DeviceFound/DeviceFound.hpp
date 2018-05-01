#ifndef BABLE_LINUX_DEVICEFOUND_HPP
#define BABLE_LINUX_DEVICEFOUND_HPP

#include <cstdint>
#include <flatbuffers/flatbuffers.h>
#include <Packet_generated.h>
#include "../EventPacket.hpp"
#include "../../../Log/Log.hpp"

namespace Packet::Events {

  class DeviceFound : public EventPacket<DeviceFound> {

  public:
    static const uint16_t event_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Events::MGMT::Code::DeviceFound;

        case Packet::Type::ASCII:
          return Events::Ascii::Code::DeviceFound;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::DeviceFound);

        default:
          throw std::runtime_error("Current type has no known id (DeviceFound).");
      }
    };

    DeviceFound(Packet::Type initial_type, Packet::Type translated_type): EventPacket(initial_type, translated_type) {
      m_address = std::array<uint8_t, 6>{0, 0, 0, 0, 0, 0};
      m_address_type = 0;
      m_rssi = 0;
      m_flags = std::array<uint8_t, 4>{0, 0, 0, 0};
      m_eir_data_length = 0;

      m_eir_flags = 0;
      m_company_id = 0;
    };

    void from_mgmt(Deserializer& deser) override {
      EventPacket::from_mgmt(deser);
      deser >> m_address >> m_address_type >> m_rssi >> m_flags >> m_eir_data_length;
      m_eir_data.resize(m_eir_data_length);
      deser >> m_eir_data;
      extract_eir(m_eir_data);
    };

    std::string to_ascii() const override {
      std::string header = EventPacket::to_ascii();
      std::stringstream payload;

      payload << "Address: " << format_bd_address(m_address) << ", "
              << "Address type: " << std::to_string(m_address_type) << ", "
              << "RSSI: " << std::to_string(m_rssi) << ", "
              << "Flags: " << std::to_string(m_flags[0]) << " " << std::to_string(m_flags[1]) << " " << std::to_string(m_flags[2]) << " " << std::to_string(m_flags[3]) << ", "
              << "EIR data length: " << std::to_string(m_eir_data_length) << ", "
              << "EIR flags: " << std::to_string(m_eir_flags) << ", "
              << "EIR UUID: " << format_uuid(m_uuid) << ", "
              << "EIR Company ID: " << std::to_string(m_company_id) << ", "
              << "EIR Device Name: " << format_device_name(m_device_name) << ", ";

      return header + ", " + payload.str();
    };

    Serializer to_flatbuffers() const override {
      std::vector<uint8_t> flags_vector(m_flags.begin(), m_flags.end());

      flatbuffers::FlatBufferBuilder builder(0);
      auto address = builder.CreateString(format_bd_address(m_address));
      auto flags = builder.CreateVector(flags_vector);
      auto uuid = builder.CreateString(format_uuid(m_uuid));
      auto device_name = builder.CreateString(format_device_name(m_device_name));

      auto payload = Schemas::CreateDeviceFound(
          builder,
          m_controller_id,
          address,
          m_address_type,
          m_rssi,
          flags,
          uuid,
          m_company_id,
          device_name
      );

      Serializer ser = build_flatbuffers_packet<Schemas::DeviceFound>(builder, payload, Schemas::Payload::DeviceFound);

      Serializer result;
      result << static_cast<uint8_t>(0xCA) << static_cast<uint8_t>(0xFE) << static_cast<uint16_t>(ser.size()) << ser;

      return result;
    }

  private:
    static std::string format_bd_address(const std::array<uint8_t, 6>& bd_address_array) {
      std::stringstream bd_address;

      for(auto it = bd_address_array.rbegin(); it != bd_address_array.rend(); ++it) {
        bd_address << RAW_HEX(*it, 2);
        if (std::next(it) != bd_address_array.rend()) {
          bd_address << ":";
        }
      }

      return bd_address.str();
    }

    static std::string format_uuid(const std::vector<uint8_t>& uuid_vector) {
      std::stringstream uuid;

      for(auto& v : uuid_vector) {
        uuid << RAW_HEX(v, 2);
      }

      return uuid.str();
    }

    static std::string format_device_name(const std::vector<uint8_t>& device_name_array) {
      std::stringstream device_name;

      for(auto& v : device_name_array) {
        device_name << char(v);
      }

      return device_name.str();
    }

    void extract_eir(const std::vector<uint8_t>& eir) {
      if (eir.size() < 3) {
        return;
      }

      for(auto it = eir.begin(); it != eir.end(); ++it) {
        uint8_t field_length = *it;
        if(++it == eir.end()) return;

        uint8_t type = *it;
        if(++it == eir.end()) return;

        field_length--;

        switch(type) {
          case 0x01: // Flags
            m_eir_flags = *it;
            break;

          case 0x02: // 16bits UUID
          case 0x03:
            m_uuid.assign(it, it + 4);
            break;

          case 0x04: // 32bits UUID
          case 0x05:
            m_uuid.assign(it, it + 8);
            break;

          case 0x06: // 128bits UUID
          case 0x07:
            m_uuid.assign(it, it + 16);
            break;

          case 0xFF: // Manufacturer Specific
            m_company_id = (*it << 8) | *(it + 1);
            break;

          case 0x09: // Device complete name
            m_device_name.assign(it, it + field_length);
            break;

          default:
            LOG.debug("Unknown EIR type received: " + std::to_string(type), "DeviceFoundEvent");
            break;
        }

        for(uint8_t i = 0; i < field_length - 1; i++) {
          if(++it == eir.end()) return;
        }
      }
    }

    std::array<uint8_t, 6> m_address{};
    uint8_t m_address_type;
    int8_t m_rssi;
    std::array<uint8_t, 4> m_flags{};
    uint16_t m_eir_data_length;
    std::vector<uint8_t> m_eir_data;

    uint8_t m_eir_flags;
    std::vector<uint8_t> m_uuid;
    uint16_t m_company_id;
    std::vector<uint8_t> m_device_name;
  };

}

#endif //BABLE_LINUX_DEVICEFOUND_HPP
