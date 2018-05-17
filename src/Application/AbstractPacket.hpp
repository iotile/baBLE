#ifndef BABLE_LINUX_ABSTRACTPACKET_HPP
#define BABLE_LINUX_ABSTRACTPACKET_HPP

#include <cstdint>
#include <utility>
#include <vector>
#include "Packets/constants.hpp"
#include "../Format/Ascii/AsciiFormat.hpp"
#include "../Format/HCI/HCIFormat.hpp"
#include "../Format/MGMT/MGMTFormat.hpp"
#include "../Format/Flatbuffers/FlatbuffersFormat.hpp"

namespace Packet {

  class AbstractPacket : public Loggable {

  public:
    static const uint16_t command_code(Packet::Type type) {
      throw std::runtime_error("command_code(Packet::Type) not defined.");
    };

    static const uint16_t event_code(Packet::Type type) {
      throw std::runtime_error("event_code(Packet::Type) not defined.");
    };

    template<typename T_CONTROLLER, typename T_COMMAND, typename T_HANDLE = uint64_t>
    static uint64_t compute_uuid(T_CONTROLLER controller_id, T_COMMAND command_code, T_HANDLE handle = 0) {
      return static_cast<uint64_t>(controller_id) << 48
          | static_cast<uint64_t>(command_code) << 32
          | static_cast<uint64_t>(handle);
    };

    virtual std::vector<uint8_t> to_bytes() const {
      switch(current_type()) {
        case Packet::Type::MGMT:
        {
          MGMTFormatBuilder builder(m_controller_id);
          return serialize(builder);
        }

        case Packet::Type::ASCII:
        {
          AsciiFormatBuilder builder;
          builder
              .add("UUID", m_uuid_request)
              .add("Native class", m_native_class)
              .add("Event code", m_event_code)
              .add("Controller ID", m_controller_id);
          return serialize(builder);
        }

        case Packet::Type::FLATBUFFERS:
        {
          FlatbuffersFormatBuilder builder(m_controller_id, m_uuid_request, m_native_class);
          return serialize(builder);
        }

        case Packet::Type::NONE:
          throw std::runtime_error("Can't serialize NONE type packet.");
      }
    };

    virtual void from_bytes(const std::vector<uint8_t>& raw_data) {
      switch(current_type()) {
        case Packet::Type::MGMT:
        {
          MGMTFormatExtractor extractor(raw_data);
          m_event_code = extractor.get_event_code();
          m_controller_id = extractor.get_controller_id();
          return unserialize(extractor);
        }

        case Packet::Type::ASCII:
        {
          AsciiFormatExtractor extractor(raw_data);
          m_controller_id = extractor.get_controller_id();
          m_uuid_request = extractor.get_uuid_request();
          return unserialize(extractor);
        }

        case Packet::Type::FLATBUFFERS:
        {
          FlatbuffersFormatExtractor extractor(raw_data);
          m_controller_id = extractor.get_controller_id();
          m_uuid_request = extractor.get_uuid_request();
          return unserialize(extractor);
        }

        case Packet::Type::NONE:
          throw std::runtime_error("Can't import data into NONE type packet.");
      }
    };

    virtual std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const {
      throw std::runtime_error("serialize(MGMTFormatBuilder&) not defined.");
    };
    virtual std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const {
      throw std::runtime_error("serialize(AsciiFormatBuilder&) not defined.");
    };
    virtual std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const {
      throw std::runtime_error("serialize(FlatbuffersFormatBuilder&) not defined.");
    };

    virtual void unserialize(MGMTFormatExtractor& extractor) {
      throw std::runtime_error("unserialize(MGMTFormatExtractor&) not defined.");
    };
    virtual void unserialize(AsciiFormatExtractor& extractor) {
      throw std::runtime_error("unserialize(AsciiFormatExtractor&) not defined.");
    };
    virtual void unserialize(FlatbuffersFormatExtractor& extractor) {
      throw std::runtime_error("unserialize(FlatbuffersFormatExtractor&) not defined.");
    };

    const Packet::Type current_type() const {
      return m_current_type;
    };

    const std::string uuid_request() const {
      return m_uuid_request;
    };

    const uint16_t controller_id() const {
      return m_controller_id;
    };

    virtual void translate() {
      m_current_type = m_current_type == m_initial_type ? m_translated_type : m_initial_type;
      after_translate();
    };
    virtual void after_translate() {};

    virtual uint64_t expected_response_uuid() {
      return 0;
    };
    virtual bool on_response_received(Packet::Type packet_type, const std::vector<uint8_t>& raw_data) {
      throw std::runtime_error("on_response_received(vector<uint8_t>&) callback not defined.");
    };

    const std::string stringify() const override {
      AsciiFormatBuilder builder;
      builder
          .add("Event code", m_event_code)
          .add("Controller ID", m_controller_id);
      std::vector<uint8_t> data = serialize(builder);
      return AsciiFormat::bytes_to_string(data);
    };

    virtual ~AbstractPacket() = default;

  protected:
    AbstractPacket(Packet::Type initial_type, Packet::Type translated_type)
        : m_initial_type(initial_type), m_translated_type(translated_type), m_current_type(m_initial_type),
          m_event_code(0), m_controller_id(NON_CONTROLLER_ID), m_uuid_request(""), m_native_class("") {};

    Packet::Type m_initial_type;
    Packet::Type m_translated_type;
    Packet::Type m_current_type;

    uint16_t m_event_code;
    uint16_t m_controller_id;
    std::string m_uuid_request;

    std::string m_native_class;

  };

}

#endif //BABLE_LINUX_ABSTRACTPACKET_HPP
