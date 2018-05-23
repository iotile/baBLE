#ifndef BABLE_LINUX_GETCONTROLLERSLIST_HPP
#define BABLE_LINUX_GETCONTROLLERSLIST_HPP

#include "../../../AbstractPacket.hpp"
#include "../../Commands/GetControllersIds/GetControllersIds.hpp"
#include "../../Commands/GetControllerInfo/GetControllerInfo.hpp"

namespace Packet::Meta {

  class GetControllersList : public AbstractPacket {

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

    void translate() override;

    std::vector<ResponseId> expected_response_ids() override;
    bool on_response_received(Packet::Type packet_type, const std::shared_ptr<AbstractExtractor>& extractor) override;

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

    std::unique_ptr<Packet::Commands::GetControllerInfo> m_controller_info_packet;
    std::unique_ptr<Packet::Commands::GetControllersIds> m_controllers_ids_packet;
    std::vector<Format::MGMT::Controller> m_controllers;
    std::vector<uint16_t> m_controllers_ids;

    uint8_t m_native_status;
    BaBLE::StatusCode m_status;

  };

}

#endif //BABLE_LINUX_GETCONTROLLERSLIST_HPP
