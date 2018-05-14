#ifndef BABLE_LINUX_BOOTSTRAP_CPP
#define BABLE_LINUX_BOOTSTRAP_CPP

#include <memory>
#include "Format/AbstractFormat.hpp"
#include "Builder/PacketBuilder.hpp"
#include "Packet/Commands/GetMGMTInfo/GetMGMTInfo.hpp"
#include "Packet/Commands/Scan/StartScan.hpp"
#include "Packet/Commands/Scan/StopScan.hpp"
#include "Packet/Commands/AddDevice/AddDevice.hpp"
#include "Packet/Commands/RemoveDevice/RemoveDevice.hpp"
#include "Packet/Commands/Disconnect/Disconnect.hpp"
#include "Packet/Commands/SetPowered/SetPowered.hpp"
#include "Packet/Commands/SetDiscoverable/SetDiscoverable.hpp"
#include "Packet/Commands/SetConnectable/SetConnectable.hpp"
#include "Packet/Events/DeviceConnected/DeviceConnected.hpp"
#include "Packet/Events/DeviceDisconnected/DeviceDisconnected.hpp"
#include "Packet/Events/DeviceFound/DeviceFound.hpp"
#include "Packet/Events/Discovering/Discovering.hpp"
#include "Packet/Events/ClassOfDeviceChanged/ClassOfDeviceChanged.hpp"
#include "Packet/Events/NewSettings/NewSettings.hpp"
#include "Packet/Commands/GetControllersList/GetControllersList.hpp"
#include "Packet/Events/ControllerAdded/ControllerAdded.hpp"
#include "Packet/Events/ControllerRemoved/ControllerRemoved.hpp"
#include "Packet/Commands/GetControllerInfo/GetControllerInfo.hpp"
#include "Packet/Commands/GetConnectedDevices/GetConnectedDevices.hpp"

using namespace std;

namespace Bootstrap {

  // MGMT builder
  void register_mgmt_packets(PacketBuilder& mgmt_builder, shared_ptr<AbstractFormat> output_format) {
    mgmt_builder
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

  // Stdio builder
  void register_stdio_packets(PacketBuilder& stdio_builder, shared_ptr<AbstractFormat> mgmt_format) {
    stdio_builder
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
