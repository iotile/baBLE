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
#include "Application/Packets/Commands/Read/Read.hpp"
#include "Application/Packets/Commands/Write/Write.hpp"
#include "Application/Packets/Commands/NotificationReceived/NotificationReceived.hpp"
#include "Application/Packets/Commands/ProbeServices/ProbeServices.hpp"
#include "Application/Packets/Commands/ProbeCharacteristics/ProbeCharacteristics.hpp"
#include "Application/Packets/Commands/WriteWithoutResponse/WriteWithoutResponse.hpp"
#include "Application/Packets/Control/Exit/Exit.hpp"
#include "Application/Packets/Control/Ready/Ready.hpp"

using namespace std;
using Packet::Meta::GetControllersList;

namespace Bootstrap {

  // Create one HCI socket per controller
  vector<shared_ptr<HCISocket>> create_hci_sockets(const shared_ptr<MGMTSocket>& mgmt_socket, const shared_ptr<HCIFormat>& hci_format) {
    // TODO: use ioctl instead
    vector<shared_ptr<HCISocket>> hci_sockets;
    Packet::Type packet_type = mgmt_socket->format()->get_packet_type();
    shared_ptr<GetControllersList> get_controllers_list_packet = make_shared<GetControllersList>(packet_type, packet_type);

    // Send GetControllersList packet through MGMT
    while (!get_controllers_list_packet->expected_response_ids().empty()){
      mgmt_socket->sync_send(get_controllers_list_packet->to_bytes());
      vector<uint8_t> raw_response = mgmt_socket->sync_receive();
      shared_ptr<AbstractExtractor> extractor = mgmt_socket->format()->create_extractor(raw_response);
      get_controllers_list_packet->on_response_received(packet_type, extractor);
    }

    vector<Format::MGMT::Controller> controllers = get_controllers_list_packet->get_controllers();
    hci_sockets.reserve(controllers.size());

    for (auto& controller : controllers) {
      hci_sockets.push_back(make_shared<HCISocket>(hci_format, controller.id, controller.address));
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
        .register_event<Packet::Events::DeviceFound>()
        .register_event<Packet::Events::Discovering>()
        .register_event<Packet::Events::ControllerAdded>()
        .register_event<Packet::Events::ControllerRemoved>()
      .set_output_format(nullptr)
        .register_event<Packet::Events::DeviceConnected>()
        .register_event<Packet::Events::DeviceDisconnected>()
        .register_event<Packet::Events::ClassOfDeviceChanged>()
        .register_event<Packet::Events::NewSettings>();
  }

  // HCI
  void register_hci_packets(PacketContainer& hci_packet_container, shared_ptr<AbstractFormat> output_format) {
    hci_packet_container
      .set_output_format(std::move(output_format))
        .register_event<Packet::Events::DeviceConnected>()
        .register_event<Packet::Events::DeviceDisconnected>()
        .register_command<Packet::Commands::Read>()
        .register_command<Packet::Commands::Write>()
        .register_command<Packet::Commands::NotificationReceived>()
        .register_command<Packet::Commands::ProbeServices>()
        .register_command<Packet::Commands::ProbeCharacteristics>();
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
        .register_command<Packet::Meta::GetControllersList>()
      .set_output_format(std::move(hci_format))
        .register_command<Packet::Commands::Read>()
        .register_command<Packet::Commands::Write>()
        .register_command<Packet::Commands::WriteWithoutResponse>()
        .register_command<Packet::Commands::ProbeServices>()
        .register_command<Packet::Commands::ProbeCharacteristics>()
      .set_output_format(nullptr)
        .register_command<Packet::Control::Exit>();
  }

}

#endif //BABLE_LINUX_BOOTSTRAP_CPP
