#ifndef BABLE_LINUX_BOOTSTRAP_CPP
#define BABLE_LINUX_BOOTSTRAP_CPP

#include <memory>
#include "Format/AbstractFormat.hpp"
#include "Application/PacketContainer/PacketContainer.hpp"
#include "Application/Packets/Commands/GetMGMTInfo/GetMGMTInfo.hpp"
#include "Application/Packets/Commands/Scan/StartScan.hpp"
#include "Application/Packets/Commands/Scan/StopScan.hpp"
#include "Application/Packets/Commands/AddDevice/AddDevice.hpp"
#include "Application/Packets/Commands/RemoveDevice/RemoveDevice.hpp"
#include "Application/Packets/Commands/Disconnect/Disconnect.hpp"
#include "Application/Packets/Commands/SetPowered/SetPowered.hpp"
#include "Application/Packets/Commands/SetDiscoverable/SetDiscoverable.hpp"
#include "Application/Packets/Commands/SetConnectable/SetConnectable.hpp"
#include "Application/Packets/Commands/GetControllersIds/GetControllersIds.hpp"
#include "Application/Packets/Commands/GetControllerInfo/GetControllerInfo.hpp"
#include "Application/Packets/Meta/GetControllersList/GetControllersList.hpp"
#include "Application/Packets/Commands/GetConnectedDevices/GetConnectedDevices.hpp"
#include "Application/Packets/Events/DeviceConnected/DeviceConnected.hpp"
#include "Application/Packets/Events/DeviceDisconnected/DeviceDisconnected.hpp"
#include "Application/Packets/Events/DeviceFound/DeviceFound.hpp"
#include "Application/Packets/Events/Discovering/Discovering.hpp"
#include "Application/Packets/Events/ClassOfDeviceChanged/ClassOfDeviceChanged.hpp"
#include "Application/Packets/Events/NewSettings/NewSettings.hpp"
#include "Application/Packets/Events/ControllerAdded/ControllerAdded.hpp"
#include "Application/Packets/Events/ControllerRemoved/ControllerRemoved.hpp"

using namespace std;
using Packet::Commands::GetControllersIds;

namespace Bootstrap {

  // Create one HCI socket per controller
  vector<shared_ptr<HCISocket>> create_hci_sockets(const shared_ptr<MGMTSocket>& mgmt_socket, const shared_ptr<HCIFormat>& hci_format) {
    vector<shared_ptr<HCISocket>> hci_sockets;
    Packet::Type packet_type = mgmt_socket->format()->packet_type();
    shared_ptr<GetControllersIds> get_controllers_ids_packet = make_shared<GetControllersIds>(packet_type, packet_type);

    // Send GetControllersIds packet through MGMT
    mgmt_socket->sync_send(get_controllers_ids_packet->to_bytes());

    // Get response containing the list of controllers ids
    vector<uint8_t> raw_response = mgmt_socket->sync_receive();
    get_controllers_ids_packet->from_bytes(raw_response);

    vector<uint16_t> controllers_ids = get_controllers_ids_packet->get_controllers_ids();
    hci_sockets.reserve(controllers_ids.size());

    for (auto& controller_id : controllers_ids) {
      hci_sockets.push_back(make_shared<HCISocket>(hci_format, controller_id));
    }

    return hci_sockets;
  }

  // MGMT
  void register_mgmt_packets(PacketContainer& mgmt_packet_container, shared_ptr<AbstractFormat> output_format) {
    mgmt_packet_container
        .set_output_format(std::move(output_format))
        .register_command<Packet::Commands::GetMGMTInfo>()
        .register_command<Packet::Commands::GetControllersIds>()
        .register_command<Packet::Commands::GetControllerInfo>()
        .register_command<Packet::Commands::GetConnectedDevices>()
        .register_command<Packet::Commands::StartScan>()
        .register_command<Packet::Commands::StopScan>()
        .register_command<Packet::Commands::AddDevice>()
        .register_command<Packet::Commands::RemoveDevice>()
        .register_command<Packet::Commands::Disconnect>()
        .register_command<Packet::Commands::SetPowered>()
        .register_command<Packet::Commands::SetDiscoverable>()
        .register_command<Packet::Commands::SetConnectable>()
        .register_event<Packet::Events::DeviceConnected>()
        .register_event<Packet::Events::DeviceDisconnected>()
        .register_event<Packet::Events::DeviceFound>()
        .register_event<Packet::Events::Discovering>()
        .register_event<Packet::Events::ControllerAdded>()
        .register_event<Packet::Events::ControllerRemoved>()
        .set_output_format(nullptr)
        .register_event<Packet::Events::ClassOfDeviceChanged>()
        .register_event<Packet::Events::NewSettings>();
  }

  // HCI
  void register_hci_packets(PacketContainer& hci_packet_container, shared_ptr<AbstractFormat> output_format) {
    hci_packet_container
        .set_output_format(std::move(output_format));
  }

  // Stdio
  void register_stdio_packets(PacketContainer& stdio_packet_container, shared_ptr<MGMTFormat> mgmt_format, shared_ptr<HCIFormat> hci_format) {
    stdio_packet_container
        .set_output_format(std::move(mgmt_format))
        .register_command<Packet::Commands::GetMGMTInfo>()
        .register_command<Packet::Commands::GetControllersIds>()
        .register_command<Packet::Commands::GetControllerInfo>()
        .register_command<Packet::Commands::GetConnectedDevices>()
        .register_command<Packet::Commands::StartScan>()
        .register_command<Packet::Commands::StopScan>()
        .register_command<Packet::Commands::AddDevice>()
        .register_command<Packet::Commands::RemoveDevice>()
        .register_command<Packet::Commands::Disconnect>()
        .register_command<Packet::Commands::SetPowered>()
        .register_command<Packet::Commands::SetDiscoverable>()
        .register_command<Packet::Commands::SetConnectable>()

        .register_command<Packet::Meta::GetControllersList>();
  }

}

#endif //BABLE_LINUX_BOOTSTRAP_CPP
