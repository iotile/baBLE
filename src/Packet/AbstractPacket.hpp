#ifndef BABLE_LINUX_ABSTRACTPACKET_HPP
#define BABLE_LINUX_ABSTRACTPACKET_HPP

#include <flatbuffers/flatbuffers.h>
#include <Packet_generated.h>
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

        case Packet::Type::FLATBUFFERS:
          return to_flatbuffers();
      }
    };

    virtual void unserialize(Deserializer& deser) {
      switch(m_current_type) {
        case Packet::Type::MGMT:
          return from_mgmt(deser);

        case Packet::Type::HCI:
          return from_hci(deser);

        case Packet::Type::ASCII:
          {
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

        case Packet::Type::FLATBUFFERS:
          return from_flatbuffers(deser);
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
    virtual Serializer to_flatbuffers() const { throw std::runtime_error("to_flatbuffers() not defined.");  };

    virtual ~AbstractPacket() = default;

  protected:
    AbstractPacket(Packet::Type initial_type, Packet::Type translated_type): m_current_type(initial_type), m_translated_type(translated_type) {};

    template<class T>
    Serializer build_flatbuffers_packet(flatbuffers::FlatBufferBuilder& builder, const flatbuffers::Offset<T>& payload, Schemas::Payload payload_type) const {
      Schemas::PacketBuilder packet_builder(builder);
      packet_builder.add_payload_type(payload_type);
      packet_builder.add_payload(payload.Union());
      auto packet = packet_builder.Finish();
      builder.Finish(packet);

      Serializer ser(builder.GetBufferPointer(), static_cast<size_t>(builder.GetSize()));
      return ser;
    }

    virtual void from_mgmt(Deserializer& deser) { throw std::runtime_error("from_mgmt(Deserializer&) not defined.");  };
    virtual void from_hci(Deserializer& deser) { throw std::runtime_error("from_hci(Deserializer&) not defined.");  };
    virtual void from_ascii(const std::vector<std::string>& params) { throw std::runtime_error("from_ascii(const std::vector<std::string>&) not defined.");  };
    virtual void from_flatbuffers(Deserializer& deser) { throw std::runtime_error("from_flatbuffers(Deserializer&) not defined.");  };

    Packet::Type m_current_type;
    Packet::Type m_translated_type;

  };

}

#endif //BABLE_LINUX_ABSTRACTPACKET_HPP
