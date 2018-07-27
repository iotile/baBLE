#include <sstream>
#include "NumberOfCompletedPackets.hpp"
#include "Transport/Socket/HCI/HCISocket.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    NumberOfCompletedPackets::NumberOfCompletedPackets()
        : ControllerToHostPacket(Packet::Id::NumberOfCompletedPackets, initial_type(), initial_packet_code(), final_packet_code(), true)
    {}

    void NumberOfCompletedPackets::unserialize(HCIFormatExtractor& extractor) {
      auto length = extractor.get_value<uint8_t>();
      auto num_connection_handle = extractor.get_value<uint8_t>();

      for (uint8_t i = 0; i < num_connection_handle; i++) {
        auto connection_handle = extractor.get_value<uint16_t>();
        auto num_complete_packets = extractor.get_value<uint16_t>();

        m_completed_packets.emplace_back(make_tuple(connection_handle, num_complete_packets));
      }
    }

    void NumberOfCompletedPackets::set_socket(AbstractSocket* socket) {
      if (m_completed_packets.empty()) {
        return;
      }

      auto hci_socket = dynamic_cast<HCISocket*>(socket);
      if (hci_socket == nullptr) {
        throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Can't downcast socket to HCISocket packet");
      }

      for (auto& info : m_completed_packets) {
        hci_socket->set_in_progress_packets(get<0>(info), get<1>(info));
      }
    }

    const string NumberOfCompletedPackets::stringify() const {
      stringstream result;

      result << "<NumberOfCompletedPackets> "
             << AbstractPacket::stringify() << ", ";

      for (auto it = m_completed_packets.begin(); it != m_completed_packets.end(); ++it) {
        result << "{ Handle: " << to_string(get<0>(*it)) << ", "
               << "Number of completed packets: " << to_string(get<1>(*it)) << "} ";
        if (next(it) != m_completed_packets.end()) {
          result << ", ";
        }
      }

      return result.str();
    }

  }

}