#ifndef BABLE_LINUX_ABSTRACTPACKET_HPP
#define BABLE_LINUX_ABSTRACTPACKET_HPP

#include "../Serializer/Serializer.hpp"
#include "../Serializer/Deserializer.hpp"
#include "constants.hpp"

namespace Packet {

  class AbstractPacket {

  public:
    virtual Serializer serialize() const {
      switch(m_current_type) {
        case Packet::Type::MGMT:
          return to_mgmt();

        case Packet::Type::HCI:
          return to_hci();

        case Packet::Type::ASCII:
          throw std::runtime_error("Can't serialize from Ascii packet.");
      }
    };

    virtual void unserialize(Deserializer& deser) {
      switch(m_current_type) {
        case Packet::Type::MGMT:
          return from_mgmt(deser);

        case Packet::Type::HCI:
          return from_hci(deser);

        case Packet::Type::ASCII:
          std::string data_str;
          deser >> data_str;

          std::stringstream data_stream(data_str);
          std::vector<std::string> params;

          while(data_stream.good()) {
            std::string substr;
            getline(data_stream, substr, Packet::Commands::Ascii::Delimiter);
            params.push_back(substr);
          }
          return from_ascii(params);
      }
    };

    virtual const Packet::Type get_type() const {
      return m_current_type;
    };

    void translate() {
      Packet::Type tmp = m_current_type;
      m_current_type = m_translated_type;
      m_translated_type = m_current_type;
      after_translate();
    };

    virtual void after_translate() {};

    virtual Serializer to_mgmt() const { throw std::runtime_error("to_mgmt() not defined.");  };
    virtual Serializer to_hci() const { throw std::runtime_error("to_hci() not defined.");  };
    virtual std::string to_ascii() const { throw std::runtime_error("to_ascii() not defined.");  };

    virtual ~AbstractPacket() = default;

  protected:
    AbstractPacket(Packet::Type initial_type, Packet::Type translated_type): m_current_type(initial_type), m_translated_type(translated_type) {};

    virtual void from_mgmt(Deserializer& deser) { throw std::runtime_error("from_mgmt(Deserializer&) not defined.");  };
    virtual void from_hci(Deserializer& deser) { throw std::runtime_error("from_hci(Deserializer&) not defined.");  };
    virtual void from_ascii(const std::vector<std::string>& params) { throw std::runtime_error("from_ascii(const std::vector<std::string>&) not defined.");  };

    Packet::Type m_current_type;
    Packet::Type m_translated_type;

  };

}

#endif //BABLE_LINUX_ABSTRACTPACKET_HPP
