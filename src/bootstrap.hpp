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
#include "Application/Packets/Events/DeviceConnected/DeviceConnected.hpp"
#include "Application/Packets/Events/DeviceDisconnected/DeviceDisconnected.hpp"
#include "Application/Packets/Events/DeviceFound/DeviceFound.hpp"
#include "Application/Packets/Events/Discovering/Discovering.hpp"
#include "Application/Packets/Events/ClassOfDeviceChanged/ClassOfDeviceChanged.hpp"
#include "Application/Packets/Events/NewSettings/NewSettings.hpp"
#include "Application/Packets/Commands/GetControllersList/GetControllersList.hpp"
#include "Application/Packets/Events/ControllerAdded/ControllerAdded.hpp"
#include "Application/Packets/Events/ControllerRemoved/ControllerRemoved.hpp"
#include "Application/Packets/Commands/GetControllerInfo/GetControllerInfo.hpp"
#include "Application/Packets/Commands/GetConnectedDevices/GetConnectedDevices.hpp"

using namespace std;

namespace Bootstrap {

  // MGMT
  void register_mgmt_packets(PacketContainer& mgmt_packet_container, shared_ptr<AbstractFormat> output_format) {
    mgmt_packet_container
      .set_output_format(std::move(output_format))
        .register_command<Packet::Commands::GetMGMTInfo>()
        .register_command<Packet::Commands::GetControllersList>()
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

  // Stdio
  void register_stdio_packets(PacketContainer& stdio_packet_container, shared_ptr<AbstractFormat> mgmt_format) {
    stdio_packet_container
      .set_output_format(std::move(mgmt_format))
        .register_command<Packet::Commands::GetMGMTInfo>()
        .register_command<Packet::Commands::GetControllersList>()
        .register_command<Packet::Commands::GetControllerInfo>()
        .register_command<Packet::Commands::GetConnectedDevices>()
        .register_command<Packet::Commands::StartScan>()
        .register_command<Packet::Commands::StopScan>()
        .register_command<Packet::Commands::AddDevice>()
        .register_command<Packet::Commands::RemoveDevice>()
        .register_command<Packet::Commands::Disconnect>()
        .register_command<Packet::Commands::SetPowered>()
        .register_command<Packet::Commands::SetDiscoverable>()
        .register_command<Packet::Commands::SetConnectable>();
  }

}

#endif //BABLE_LINUX_BOOTSTRAP_CPP
