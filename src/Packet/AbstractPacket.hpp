#ifndef BABLE_LINUX_ABSTRACTPACKET_HPP
#define BABLE_LINUX_ABSTRACTPACKET_HPP

#include <cstdint>
#include <utility>
#include <vector>
#include "constants.hpp"
#include "../Format/MGMT/constants.hpp"
#include "../Format/Ascii/constants.hpp"
#include "../Format/Ascii/AsciiFormat.hpp"
#include "../Format/MGMT/MGMTFormat.hpp"
#include "../Format/Flatbuffers/FlatbuffersFormatExtractor.hpp"
#include "../Format/Flatbuffers/FlatbuffersFormatBuilder.hpp"

namespace Packet {

  class AbstractPacket : public Loggable {

  public:
    static const uint16_t command_code(Packet::Type type) {
      throw std::runtime_error("command_code(Packet::Type) not defined.");
    }

    static const uint16_t event_code(Packet::Type type) {
      throw std::runtime_error("event_code(Packet::Type) not defined.");
    }

    virtual std::vector<uint8_t> serialize() const {
      switch(m_current_type) {
        case Packet::Type::MGMT:
        {
          MGMTFormatBuilder builder(m_controller_id);
          return serialize(builder);
        }

        case Packet::Type::ASCII:
        {
          AsciiFormatBuilder builder;
          builder
              .add("Event code", m_event_code)
              .add("Controller ID", m_controller_id);
          return serialize(builder);
        }

        case Packet::Type::FLATBUFFERS:
        {
          FlatbuffersFormatBuilder builder(0);
          return serialize(builder);
        }
      }
    };

    virtual void import(const std::vector<uint8_t>& raw_data) {
      switch(m_current_type) {
        case Packet::Type::MGMT:
        {
          MGMTFormatExtractor extractor(raw_data);
          m_event_code = extractor.get_event_code();
          m_controller_id = extractor.get_controller_id();
          return import(extractor);
        }

        case Packet::Type::ASCII:
        {
          AsciiFormatExtractor extractor(raw_data);
          return import(extractor);
        }

        case Packet::Type::FLATBUFFERS:
        {
          FlatbuffersFormatExtractor extractor(raw_data);
          return import(extractor);
        }
      }
    };

    virtual const Packet::Type current_type() const {
      return m_current_type;
    };

    void translate() {
      std::swap(m_current_type, m_translated_type);
      after_translate();
    };
    virtual void after_translate() {};

    virtual std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const {
      throw std::runtime_error("serialize(MGMTFormatBuilder&) not defined.");
    };
    virtual std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const {
      throw std::runtime_error("serialize(AsciiFormatBuilder&) not defined.");
    };
    virtual std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const {
      throw std::runtime_error("serialize(FlatbuffersFormatBuilder&) not defined.");
    };

    virtual void import(MGMTFormatExtractor& extractor) {
      throw std::runtime_error("import(MGMTFormatExtractor&) not defined.");
    };
    virtual void import(AsciiFormatExtractor& extractor) {
      throw std::runtime_error("import(AsciiFormatExtractor&) not defined.");
    };
    virtual void import(FlatbuffersFormatExtractor& extractor) {
      throw std::runtime_error("import(FlatbuffersFormatExtractor&) not defined.");
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
        : m_current_type(initial_type), m_translated_type(translated_type), m_event_code(0), m_controller_id(0) {};

    Packet::Type m_current_type;
    Packet::Type m_translated_type;

    uint16_t m_event_code;
    uint16_t m_controller_id;

  };

}

#endif //BABLE_LINUX_ABSTRACTPACKET_HPP
