#ifndef BABLE_LINUX_GETCONTROLLERSLIST_HPP
#define BABLE_LINUX_GETCONTROLLERSLIST_HPP

#include "../CommandPacket.hpp"
#include "../GetControllerInfo/GetControllerInfo.hpp"

namespace Packet::Commands {

  class GetControllersList : public CommandPacket<GetControllersList> {

  public:
    static const uint16_t command_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::CommandCode::GetControllersList;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::GetControllersList;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::GetControllersList);

        case Packet::Type::NONE:
          return 0;
      }
    };

    GetControllersList(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(AsciiFormatExtractor& extractor) override;
    void unserialize(FlatbuffersFormatExtractor& extractor) override;
    void unserialize(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

    void translate() override;

    uint64_t expected_response_uuid() override;
    bool on_response_received(Packet::Type packet_type, const std::vector<uint8_t>& raw_data) override;

  private:
    bool m_waiting_controllers_info;
    uint16_t m_current_index;

    std::unique_ptr<GetControllerInfo> m_controller_info_packet;
    std::vector<uint16_t> m_controllers_ids;
    std::vector<Format::MGMT::Controller> m_controllers;

  };

}

#endif //BABLE_LINUX_GETCONTROLLERSLIST_HPP
