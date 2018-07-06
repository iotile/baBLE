#ifndef BABLE_LINUX_ERRORRESPONSE_HPP
#define BABLE_LINUX_ERRORRESPONSE_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Errors {

    class ErrorResponse : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return Format::HCI::AttributeCode::ErrorResponse;
      };

      static const uint16_t final_packet_code() {
        return initial_packet_code();
      };

      ErrorResponse();

      void unserialize(HCIFormatExtractor& extractor) override;

      const std::string stringify() const override;

      const PacketUuid get_uuid() const override;

      inline uint8_t get_opcode() const {
        return m_opcode;
      };

      inline uint16_t get_handle() const {
        return m_handle;
      };

      inline Format::HCI::AttributeErrorCode get_error_code() const {
        return m_error_code;
      };

    private:
      uint8_t m_opcode;
      uint16_t m_handle;
      Format::HCI::AttributeErrorCode m_error_code;

    };

  }

}

#endif //BABLE_LINUX_ERRORRESPONSE_HPP
