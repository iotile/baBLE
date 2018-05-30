#ifndef BABLE_LINUX_GETCONTROLLERSLIST_HPP
#define BABLE_LINUX_GETCONTROLLERSLIST_HPP

#include "../../../AbstractPacket.hpp"
#include "../../Commands/GetControllersIds/GetControllersIdsRequest.hpp"
#include "../../Commands/GetControllersIds/GetControllersIdsResponse.hpp"
#include "../../Commands/GetControllerInfo/GetControllerInfoRequest.hpp"
#include "../../Commands/GetControllerInfo/GetControllerInfoResponse.hpp"

namespace Packet::Meta {

class GetControllersList : public AbstractPacket, std::enable_shared_from_this<GetControllersList> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          throw std::invalid_argument("'GetControllersList' packet is a meta packet, can't be a MGMT packet.");

        case Packet::Type::HCI:
          throw std::invalid_argument("'GetControllersList' packet is a meta packet, can't be a HCI packet.");

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::GetControllersList;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(BaBLE::Payload::GetControllersList);

        case Packet::Type::NONE:
          return 0;
      }
    };

    GetControllersList(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(AsciiFormatExtractor& extractor) override;
    void unserialize(FlatbuffersFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

    void before_sent(const std::shared_ptr<PacketRouter>& router) override;
    std::shared_ptr<AbstractPacket> on_controllers_ids_response_received(std::shared_ptr<AbstractPacket> packet);
    std::shared_ptr<AbstractPacket> on_controller_info_response_received(std::shared_ptr<AbstractPacket> packet);

    inline const std::vector<Format::MGMT::Controller>& get_controllers() const {
      return m_controllers;
    }

  private:
    enum SubPacket {
      GetControllersIds,
      GetControllerInfo,
      None
    };

    SubPacket m_waiting_response;
    uint16_t m_current_index;

    std::unique_ptr<Packet::Commands::GetControllerInfoRequest> m_controller_info_request_packet;
    std::unique_ptr<Packet::Commands::GetControllersIdsRequest> m_controllers_ids_request_packet;

    std::vector<Format::MGMT::Controller> m_controllers;
    std::vector<uint16_t> m_controllers_ids;

  };

}

#endif //BABLE_LINUX_GETCONTROLLERSLIST_HPP
