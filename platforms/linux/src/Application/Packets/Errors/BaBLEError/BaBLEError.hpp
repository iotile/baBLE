#ifndef BABLE_LINUX_BABLEERROR_HPP
#define BABLE_LINUX_BABLEERROR_HPP

#include "../../Base/HostOnlyPacket.hpp"
#include "../../../../Exceptions/BaBLEException.hpp"

namespace Packet {

  namespace Errors {

    class BaBLEError : public HostOnlyPacket {

    public:
      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::BaBLEError);
      };

      explicit BaBLEError(const Exceptions::BaBLEException& exception);

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      std::string m_message;

    };

  }

}

#endif //BABLE_LINUX_BABLEERROR_HPP
