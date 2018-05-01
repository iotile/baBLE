#ifndef BABLE_LINUX_PACKETBUILDER_HPP
#define BABLE_LINUX_PACKETBUILDER_HPP

#include <functional>
#include <memory>
#include <iostream>
#include "../Packet/constants.hpp"
#include "../Packet/AbstractPacket.hpp"
#include "../Format/AbstractFormat.hpp"

class PacketBuilder {

  using PacketConstructor = std::function<std::unique_ptr<Packet::AbstractPacket>()>;

public:
  explicit PacketBuilder(std::shared_ptr<AbstractFormat> building_format) {
    m_building_format = std::move(building_format);
    m_output_format = nullptr;
  };

  PacketBuilder(std::shared_ptr<AbstractFormat> building_format, std::shared_ptr<AbstractFormat> output_format) {
    m_building_format = std::move(building_format);
    m_output_format = std::move(output_format);
  };

  PacketBuilder& set_output_format(std::shared_ptr<AbstractFormat> output_format) {
    m_output_format = std::move(output_format);

    return *this;
  };

  template<class T>
  PacketBuilder& register_command() {
    if (m_output_format == nullptr) {
      throw std::runtime_error("No output format specified in PacketBuilder. Can't register command.");
    }

    const Packet::Type initial_type = m_building_format->packet_type();
    uint16_t command_code = T::command_code(initial_type);

    auto it = m_commands.find(command_code);
    if (it != m_commands.end()) {
      throw std::invalid_argument("Packet already registered");
    }

    const Packet::Type translated_type = m_output_format->packet_type();

    m_commands.emplace(command_code, [initial_type, translated_type](){
      return std::make_unique<T>(initial_type, translated_type);
    });

    return *this;
  };

  template<class T>
  PacketBuilder& register_event() {
    if (m_output_format == nullptr) {
      throw std::runtime_error("No output format specified in PacketBuilder. Can't register event.");
    }

    const Packet::Type initial_type = m_building_format->packet_type();
    uint16_t event_code = T::event_code(initial_type);

    auto it = m_events.find(event_code);
    if (it != m_events.end()) {
      throw std::invalid_argument("Packet already registered");
    }

    const Packet::Type translated_type = m_output_format->packet_type();

    m_events.emplace(event_code, [initial_type, translated_type](){
      return std::make_unique<T>(initial_type, translated_type);
    });

    return *this;
  };

  std::unique_ptr<Packet::AbstractPacket> build(const std::vector<uint8_t>& raw_data) {
    uint16_t type_code = m_building_format->extract_type_code(raw_data);

    if (m_building_format->is_command(type_code)) {
      return build_command(raw_data);
    } else if (m_building_format->is_event(type_code)) {
      return build_event(type_code, raw_data);
    } else {
      throw std::invalid_argument("Given data to build a packet has no known type.");
    }
  };

  std::unique_ptr<Packet::AbstractPacket> build_command(const std::vector<uint8_t>& raw_data) {
    uint16_t command_code = m_building_format->extract_command_code(raw_data);

    auto it = m_commands.find(command_code);
    if (it == m_commands.end()) {
      throw std::invalid_argument("Command code not found in PacketBuilder registry: " + std::to_string(command_code));
    }

    PacketConstructor fn = it->second;
    std::unique_ptr<Packet::AbstractPacket> packet = fn();
    packet->import(raw_data);
    return packet;
  }

  std::unique_ptr<Packet::AbstractPacket> build_event(uint16_t event_code, const std::vector<uint8_t>& raw_data) {
    auto it = m_events.find(event_code);
    if (it == m_events.end()) {
      throw std::invalid_argument("Event code not found in PacketBuilder registry: " + std::to_string(event_code));
    }

    // Get command from command_code
    PacketConstructor fn = it->second;
    std::unique_ptr<Packet::AbstractPacket> packet = fn();
    packet->import(raw_data);
    return packet;
  }

private:
  std::shared_ptr<AbstractFormat> m_building_format;
  std::shared_ptr<AbstractFormat> m_output_format;

  std::unordered_map<uint16_t, PacketConstructor> m_commands;
  std::unordered_map<uint16_t, PacketConstructor> m_events;

};

#endif //BABLE_LINUX_PACKETBUILDER_HPP
