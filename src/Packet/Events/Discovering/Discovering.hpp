#ifndef BABLE_LINUX_DISCOVERING_HPP
#define BABLE_LINUX_DISCOVERING_HPP

#include <cstdint>
#include "../EventPacket.hpp"
#include "../../../Serializer/Deserializer.hpp"

namespace Packet::Events {

  class Discovering : public EventPacket<Discovering> {

  public:
    static const uint16_t event_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Events::MGMT::Code::Discovering;

        case Packet::Type::ASCII:
          return Events::Ascii::Code::Discovering;

        default:
          throw std::runtime_error("Current type has no known id.");
      }
    };

    Discovering(Packet::Type initial_type, Packet::Type translated_type): EventPacket(initial_type, translated_type) {
      m_address_type = 0;
      m_discovering = 0;
    };

    void from_mgmt(Deserializer& deser) override {
      EventPacket::from_mgmt(deser);
      deser >> m_address_type >> m_discovering;
    };

    std::string to_ascii() const override {
      std::string header = EventPacket::to_ascii();
      std::stringstream payload;

      payload << "Address type: " << std::to_string(m_address_type) << ", "
              << "Discovering: " << std::to_string(m_discovering);

      return header + ", " + payload.str();
    };

  private:
    uint8_t m_address_type;
    uint8_t m_discovering;
  };

}

#endif //BABLE_LINUX_DISCOVERING_HPP
