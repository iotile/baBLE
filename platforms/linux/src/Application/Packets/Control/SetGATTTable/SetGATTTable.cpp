#include <sstream>
#include "utils/string_formats.hpp"
#include "SetGATTTable.hpp"

using namespace std;

namespace Packet {

  namespace Control {

    SetGATTTable::SetGATTTable()
        : HostOnlyPacket(Packet::Id::SetGATTTable, initial_packet_code()) {}

    void SetGATTTable::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::SetGATTTable*>();

      auto raw_services = payload->services();
      auto raw_characteristics = payload->characteristics();

      for (uint32_t i = 0; i < raw_services->Length(); i++) {
        auto raw_service = raw_services->Get(i);

        Format::HCI::Service service{
          raw_service->handle(),
          raw_service->group_end_handle(),
          Utils::extract_uuid(raw_service->uuid()->str())
        };

        m_services.push_back(service);
      }

      for (uint32_t i = 0; i < raw_characteristics->Length(); i++) {
        auto raw_characteristic = raw_characteristics->Get(i);

        uint8_t properties = 0;
        if (raw_characteristic->indicate()) properties |= 1 << 5;
        if (raw_characteristic->notify()) properties |= 1 << 4;
        if (raw_characteristic->write()) properties |= 1 << 3;
        if (raw_characteristic->read()) properties |= 1 << 1;
        if (raw_characteristic->broadcast()) properties |= 1 << 0;

        uint16_t configuration = 0;
        if (raw_characteristic->indication_enabled()) configuration |= 1 << 1;
        if (raw_characteristic->notification_enabled()) configuration |= 1 << 0;

        Format::HCI::Characteristic characteristic{
            raw_characteristic->handle(),
            properties,
            raw_characteristic->value_handle(),
            raw_characteristic->config_handle(),
            configuration,
            Utils::extract_uuid(raw_characteristic->uuid()->str())  // TODO: verify if uuid order is ok
        };

        m_characteristics.push_back(characteristic);
      }
    }

    vector<uint8_t> SetGATTTable::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateSetGATTTable(builder);

      return builder.build(payload, BaBLE::Payload::SetGATTTable);
    }

    const string SetGATTTable::stringify() const {
      stringstream result;

      result << "<SetGATTTable> "
             << AbstractPacket::stringify() << ", "
             << "Services: ";

      for (auto& service : m_services) {
        result << "{ Handle: " << to_string(service.handle) << ", "
               << "Group end handle: " << to_string(service.group_end_handle) << ", "
               << "UUID: " << Utils::format_uuid(service.uuid) << "} " << ", ";
      }

      result << "Characteristics: ";
      for (auto it = m_characteristics.begin(); it != m_characteristics.end(); ++it) {
        result << "{ Handle: " << to_string(it->handle) << ", "
               << "Properties: " << to_string(it->properties) << ", "
               << "Value handle: " << to_string(it->value_handle) << ", "
               << "Config handle: " << to_string(it->config_handle) << ", "
               << "Configuration: " << to_string(it->configuration) << ", "
               << "UUID: " << Utils::format_uuid(it->uuid) << "} ";
        if (next(it) != m_characteristics.end()) {
          result << ", ";
        }
      }

      return result.str();
    }

  }

}