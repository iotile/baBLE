#ifndef BABLE_LINUX_BABLEERROR_HPP
#define BABLE_LINUX_BABLEERROR_HPP

#include "../../Base/HostOnlyPacket.hpp"
#include "../../../../Exceptions/AbstractException.hpp"

namespace Packet {

  namespace Errors {

    class BaBLEError : public HostOnlyPacket {

    public:
      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::BaBLEError);
      };

      explicit BaBLEError(const Exceptions::AbstractException& exception);

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      Exceptions::Type m_type;
      std::string m_message;
      std::string m_name;

    };

  }

}

#endif //BABLE_LINUX_BABLEERROR_HPP
