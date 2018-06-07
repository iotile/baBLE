#ifndef BABLE_LINUX_GETCONTROLLERSLIST_HPP
#define BABLE_LINUX_GETCONTROLLERSLIST_HPP

#include "../../Base/HostOnlyPacket.hpp"
#include "../../Commands/GetControllersIds/GetControllersIdsRequest.hpp"
#include "../../Commands/GetControllerInfo/GetControllerInfoRequest.hpp"

namespace Packet {

  namespace Meta {

    class GetControllersList : public HostOnlyPacket {

    public:
      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::GetControllersList);
      };

      GetControllersList();

      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void prepare(const std::shared_ptr<PacketRouter>& router) override;
      std::shared_ptr<AbstractPacket> on_controllers_ids_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                           const std::shared_ptr<AbstractPacket>& packet);
      std::shared_ptr<AbstractPacket> on_controller_info_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                           const std::shared_ptr<AbstractPacket>& packet);

      inline const std::vector<Format::MGMT::Controller>& get_controllers() const {
        return m_controllers;
      }

    private:
      Packet::Id m_waiting_response;
      uint16_t m_current_index;

      std::shared_ptr<Packet::Commands::GetControllerInfoRequest> m_controller_info_request_packet;
      std::shared_ptr<Packet::Commands::GetControllersIdsRequest> m_controllers_ids_request_packet;

      std::vector<Format::MGMT::Controller> m_controllers;
      std::vector<uint16_t> m_controllers_ids;

    };

  }

}

#endif //BABLE_LINUX_GETCONTROLLERSLIST_HPP
