#ifndef BABLE_LINUX_BOOTSTRAP_CPP
#define BABLE_LINUX_BOOTSTRAP_CPP

#include <memory>
#include "Format/AbstractFormat.hpp"
#include "Application/PacketBuilder/PacketBuilder.hpp"
#include "Application/Packets/Commands/GetMGMTInfo/GetMGMTInfoRequest.hpp"
#include "Application/Packets/Commands/GetMGMTInfo/GetMGMTInfoResponse.hpp"
#include "Application/Packets/Commands/StartScan/StartScanRequest.hpp"
#include "Application/Packets/Commands/StartScan/StartScanResponse.hpp"
#include "Application/Packets/Commands/StopScan/StopScanRequest.hpp"
#include "Application/Packets/Commands/StopScan/StopScanResponse.hpp"
#include "Application/Packets/Commands/AddDevice/AddDeviceRequest.hpp"
#include "Application/Packets/Commands/AddDevice/AddDeviceResponse.hpp"
#include "Application/Packets/Commands/RemoveDevice/RemoveDeviceRequest.hpp"
#include "Application/Packets/Commands/RemoveDevice/RemoveDeviceResponse.hpp"
#include "Application/Packets/Commands/Disconnect/DisconnectRequest.hpp"
#include "Application/Packets/Commands/Disconnect/DisconnectResponse.hpp"
#include "Application/Packets/Commands/SetPowered/SetPoweredRequest.hpp"
#include "Application/Packets/Commands/SetPowered/SetPoweredResponse.hpp"
#include "Application/Packets/Commands/SetDiscoverable/SetDiscoverableRequest.hpp"
#include "Application/Packets/Commands/SetDiscoverable/SetDiscoverableResponse.hpp"
#include "Application/Packets/Commands/SetConnectable/SetConnectableRequest.hpp"
#include "Application/Packets/Commands/SetConnectable/SetConnectableResponse.hpp"
#include "Application/Packets/Commands/GetControllersIds/GetControllersIdsRequest.hpp"
#include "Application/Packets/Commands/GetControllersIds/GetControllersIdsResponse.hpp"
#include "Application/Packets/Commands/GetControllerInfo/GetControllerInfoRequest.hpp"
#include "Application/Packets/Commands/GetControllerInfo/GetControllerInfoResponse.hpp"
#include "Application/Packets/Commands/Read/ReadRequest.hpp"
#include "Application/Packets/Commands/Read/ReadResponse.hpp"
#include "Application/Packets/Commands/Write/WriteRequest.hpp"
#include "Application/Packets/Commands/Write/WriteResponse.hpp"
#include "Application/Packets/Commands/NotificationReceived/NotificationReceived.hpp"
#include "Application/Packets/Meta/ProbeServices/ProbeServices.hpp"
#include "Application/Packets/Meta/ProbeCharacteristics/ProbeCharacteristics.hpp"
#include "Application/Packets/Commands/WriteWithoutResponse/WriteWithoutResponse.hpp"
#include "Application/Packets/Commands/GetConnectedDevices/GetConnectedDevicesRequest.hpp"
#include "Application/Packets/Commands/GetConnectedDevices/GetConnectedDevicesResponse.hpp"
#include "Application/Packets/Events/DeviceConnected/DeviceConnected.hpp"
#include "Application/Packets/Events/DeviceDisconnected/DeviceDisconnected.hpp"
#include "Application/Packets/Events/DeviceFound/DeviceFound.hpp"
#include "Application/Packets/Events/Discovering/Discovering.hpp"
#include "Application/Packets/Events/ClassOfDeviceChanged/ClassOfDeviceChanged.hpp"
#include "Application/Packets/Events/NewSettings/NewSettings.hpp"
#include "Application/Packets/Events/ControllerAdded/ControllerAdded.hpp"
#include "Application/Packets/Events/ControllerRemoved/ControllerRemoved.hpp"
#include "Application/Packets/Events/DeviceAdded/DeviceAdded.hpp"
#include "Application/Packets/Events/DeviceRemoved/DeviceRemoved.hpp"
#include "Application/Packets/Events/LEAdvertisingReport/LEAdvertisingReport.hpp"
#include "Application/Packets/Events/LEReadRemoteUsedFeaturesComplete/LEReadRemoteUsedFeaturesComplete.hpp"
#include "Application/Packets/Meta/GetControllersList/GetControllersList.hpp"
#include "Application/Packets/Control/Exit/Exit.hpp"
#include "Application/Packets/Control/Ready/Ready.hpp"

using namespace std;
using Packet::Meta::GetControllersList;

namespace Bootstrap {

  // Create one HCI socket per Bluetooth controller
  vector<shared_ptr<HCISocket>> create_hci_sockets(const shared_ptr<MGMTSocket>& mgmt_socket, const shared_ptr<HCIFormat>& hci_format) {
    vector<shared_ptr<HCISocket>> hci_sockets;

//    Packet::Type mgmt_packet_type = mgmt_socket->format()->get_packet_type();
//    shared_ptr<GetControllersList> get_controllers_list_packet = make_shared<GetControllersList>(Packet::Type::NONE, mgmt_packet_type);
//
//    // Send GetControllersList packet through MGMT
//    while (!get_controllers_list_packet->expected_response_ids().empty()){
//      mgmt_socket->sync_send(get_controllers_list_packet->to_bytes());
//      vector<uint8_t> raw_response = mgmt_socket->sync_receive();
//
//      shared_ptr<AbstractExtractor> extractor = mgmt_socket->format()->create_extractor(raw_response);
//      get_controllers_list_packet->on_response_received(mgmt_packet_type, extractor);
//    }

    // Get controllers list from GetControllersList packet
//    vector<Format::MGMT::Controller> controllers = get_controllers_list_packet->get_controllers();
    std::array<uint8_t, 6> address = {0x30, 0xF3, 0xF6, 0x44, 0xE2, 0x48};
    vector<Format::MGMT::Controller> controllers = {
        Format::MGMT::Controller{
            0,
            address
        }
    };
    hci_sockets.reserve(controllers.size());

    // Create HCI sockets
    for (auto& controller : controllers) {
      hci_sockets.push_back(make_shared<HCISocket>(hci_format, controller.id, controller.address));
    }

    return hci_sockets;
  }

  // MGMT
  void register_mgmt_packets(PacketBuilder& mgmt_packet_builder, shared_ptr<AbstractFormat> output_format) {
    mgmt_packet_builder
      .set_output_format(std::move(output_format))
        .register_command<Packet::Commands::GetMGMTInfoResponse>()
        .register_command<Packet::Commands::GetControllersIdsResponse>()
        .register_command<Packet::Commands::GetControllerInfoResponse>()
        .register_command<Packet::Commands::GetConnectedDevicesResponse>()
        .register_command<Packet::Commands::StartScanResponse>()
        .register_command<Packet::Commands::StopScanResponse>()
        .register_command<Packet::Commands::AddDeviceResponse>()
        .register_command<Packet::Commands::RemoveDeviceResponse>()
        .register_command<Packet::Commands::DisconnectResponse>()
        .register_command<Packet::Commands::SetPoweredResponse>()
        .register_command<Packet::Commands::SetDiscoverableResponse>()
        .register_command<Packet::Commands::SetConnectableResponse>()
        .register_event<Packet::Events::DeviceFound>()
        .register_event<Packet::Events::Discovering>()
        .register_event<Packet::Events::ControllerAdded>()
        .register_event<Packet::Events::ControllerRemoved>()
        .register_event<Packet::Events::DeviceAdded>()
        .register_event<Packet::Events::DeviceRemoved>()
      .set_output_format(nullptr)
        .register_event<Packet::Events::DeviceConnected>()
        .register_event<Packet::Events::DeviceDisconnected>()
        .register_event<Packet::Events::ClassOfDeviceChanged>()
        .register_event<Packet::Events::NewSettings>();
  }

  // HCI
  void register_hci_packets(PacketBuilder& hci_packet_builder, shared_ptr<AbstractFormat> output_format) {
    hci_packet_builder
      .set_output_format(std::move(output_format))
        .register_event<Packet::Events::DeviceConnected>()
        .register_event<Packet::Events::DeviceDisconnected>()
        .register_command<Packet::Commands::ReadResponse>()
        .register_command<Packet::Commands::WriteResponse>()
        .register_command<Packet::Commands::NotificationReceived>()
        .register_command<Packet::Errors::ErrorResponse>()
      .set_output_format(nullptr)
        .register_command<Packet::Commands::ReadByGroupTypeResponse>()
        .register_command<Packet::Commands::ReadByTypeResponse>()
        .register_command<Packet::Commands::ReadRequest>()
        .register_command<Packet::Commands::ReadByGroupTypeRequest>()
        .register_event<Packet::Events::LEAdvertisingReport>()
        .register_event<Packet::Events::LEReadRemoteUsedFeaturesComplete>();
  }

  // Stdio
  void register_stdio_packets(PacketBuilder& stdio_packet_builder, shared_ptr<MGMTFormat> mgmt_format, shared_ptr<HCIFormat> hci_format) {
    stdio_packet_builder
      .set_output_format(std::move(mgmt_format))
        .register_command<Packet::Commands::GetMGMTInfoRequest>()
        .register_command<Packet::Commands::GetControllersIdsRequest>()
        .register_command<Packet::Commands::GetControllerInfoRequest>()
        .register_command<Packet::Commands::GetConnectedDevicesRequest>()
        .register_command<Packet::Commands::StartScanRequest>()
        .register_command<Packet::Commands::StopScanRequest>()
        .register_command<Packet::Commands::AddDeviceRequest>()
        .register_command<Packet::Commands::RemoveDeviceRequest>()
        .register_command<Packet::Commands::DisconnectRequest>()
        .register_command<Packet::Commands::SetPoweredRequest>()
        .register_command<Packet::Commands::SetDiscoverableRequest>()
        .register_command<Packet::Commands::SetConnectableRequest>()
        .register_command<Packet::Meta::GetControllersList>()
      .set_output_format(std::move(hci_format))
        .register_command<Packet::Commands::ReadRequest>()
        .register_command<Packet::Commands::WriteRequest>()
        .register_command<Packet::Commands::WriteWithoutResponse>()
        .register_command<Packet::Meta::ProbeServices>()
        .register_command<Packet::Meta::ProbeCharacteristics>()
      .set_output_format(stdio_packet_builder.get_building_format())
        .register_command<Packet::Control::Exit>();
  }

}

#endif //BABLE_LINUX_BOOTSTRAP_CPP