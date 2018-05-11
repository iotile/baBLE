import subprocess
import sys
import time
import flatbuffers

from Schemas import Packet, Payload, GetMGMTInfo, StartScan, StopScan, Discovering, DeviceFound, BaBLEError, StatusCode,\
                    AddDevice, DeviceConnected, RemoveDevice, Disconnect, DeviceDisconnected, SetPowered, SetDiscoverable,\
                    SetConnectable, GetControllersList, ControllerAdded, ControllerRemoved, GetControllerInfo

def status_code_to_string(status_code):
    if status_code == StatusCode.StatusCode().Success:
        return "Success"
    elif status_code == StatusCode.StatusCode().SocketError:
        return "SocketError"
    elif status_code == StatusCode.StatusCode().NotFound:
        return "NotFound"
    elif status_code == StatusCode.StatusCode().WrongFormat:
        return "WrongFormat"
    elif status_code == StatusCode.StatusCode().InvalidCommand:
        return "InvalidCommand"
    elif status_code == StatusCode.StatusCode().Unknown:
        return "Unknown"
    elif status_code == StatusCode.StatusCode().Rejected:
        return "Rejected"
    elif status_code == StatusCode.StatusCode().Denied:
        return "Denied"
    elif status_code == StatusCode.StatusCode().Cancelled:
        return "Cancelled"
    elif status_code == StatusCode.StatusCode().NotPowered:
        return "NotPowered"
    elif status_code == StatusCode.StatusCode().Failed:
        return "Failed"
    elif status_code == StatusCode.StatusCode().NotConnected:
        return "NotConnected"
    else:
        return "-"

process = subprocess.Popen(["./build/debug/baBLE_linux"],
                           stdout=subprocess.PIPE, stdin=subprocess.PIPE,
                           bufsize=0,
                           universal_newlines=False)

#### ASCII
# process.stdin.write(b'\xCA\xFE' + len("1").to_bytes(2, byteorder='little') + bytes("2,0,7", encoding="utf-8"))
# process.stdin.write(bytes("2,0", encoding="utf-8"))
# time.sleep(2)
# process.stdin.write("5,0")

#### Flatbuffers
builder = flatbuffers.Builder(0)

## StartScan
# StartScan.StartScanStart(builder)
# StartScan.StartScanAddControllerId(builder, 0)
# StartScan.StartScanAddAddressType(builder, 0x07)
# payload = StartScan.StartScanEnd(builder)

## Stop scan
# StopScan.StopScanStart(builder)
# StopScan.StopScanAddControllerId(builder, 0)
# StopScan.StopScanAddAddressType(builder, 0x07)
# payload = StopScan.StopScanEnd(builder)

## AddDevice
# AddDevice.AddDeviceStartAddressVector(builder, 6)
# address_list = [0xC4, 0xF0, 0xA5, 0xE6, 0x8A, 0x91]
# for element in address_list:
#     builder.PrependByte(element)
# address = builder.EndVector(6)
#
# AddDevice.AddDeviceStart(builder)
# AddDevice.AddDeviceAddControllerId(builder, 0)
# AddDevice.AddDeviceAddAddress(builder, address)
# AddDevice.AddDeviceAddAddressType(builder, 2)
# payload = AddDevice.AddDeviceEnd(builder)

## RemoveDevice
# RemoveDevice.RemoveDeviceStartAddressVector(builder, 6)
# address_list = [0xC4, 0xF0, 0xA5, 0xE6, 0x8A, 0x91]
# for element in address_list:
#     builder.PrependByte(element)
# address = builder.EndVector(6)
#
# RemoveDevice.RemoveDeviceStart(builder)
# RemoveDevice.RemoveDeviceAddControllerId(builder, 0)
# RemoveDevice.RemoveDeviceAddAddress(builder, address)
# RemoveDevice.RemoveDeviceAddAddressType(builder, 2)
# payload = RemoveDevice.RemoveDeviceEnd(builder)

## Disconnect
# Disconnect.DisconnectStartAddressVector(builder, 6)
# address_list = [0xC4, 0xF0, 0xA5, 0xE6, 0x8A, 0x91]
# for element in address_list:
#     builder.PrependByte(element)
# address = builder.EndVector(6)
#
# Disconnect.DisconnectStart(builder)
# Disconnect.DisconnectAddControllerId(builder, 0)
# Disconnect.DisconnectAddAddress(builder, address)
# Disconnect.DisconnectAddAddressType(builder, 2)
# payload = Disconnect.DisconnectEnd(builder)

## SetPowered
# SetPowered.SetPoweredStart(builder)
# SetPowered.SetPoweredAddControllerId(builder, 0)
# SetPowered.SetPoweredAddState(builder, False)
# payload = SetPowered.SetPoweredEnd(builder)


## SetDiscoverable
# SetDiscoverable.SetDiscoverableStart(builder)
# SetDiscoverable.SetDiscoverableAddControllerId(builder, 0)
# SetDiscoverable.SetDiscoverableAddState(builder, False)
# payload = SetDiscoverable.SetDiscoverableEnd(builder)


## SetConnectable
# SetConnectable.SetConnectableStart(builder)
# SetConnectable.SetConnectableAddControllerId(builder, 0)
# SetConnectable.SetConnectableAddState(builder, False)
# payload = SetConnectable.SetConnectableEnd(builder)

## GetControllersList
# GetControllersList.GetControllersListStart(builder)
# payload = GetControllersList.GetControllersListEnd(builder)

## GetControllerInfo
GetControllerInfo.GetControllerInfoStart(builder)
GetControllerInfo.GetControllerInfoAddControllerId(builder, 0)
payload = GetControllerInfo.GetControllerInfoEnd(builder)

Packet.PacketStart(builder)
Packet.PacketAddPayloadType(builder, Payload.Payload().GetControllerInfo)
Packet.PacketAddPayload(builder, payload)
packet = Packet.PacketEnd(builder)

builder.Finish(packet)

buf = builder.Output()
buf = b'\xCA\xFE' + len(buf).to_bytes(2, byteorder='little') + buf
process.stdin.write(buf)

header_length = 4

try:
    while True:
        header = bytearray()
        while len(header) < header_length:
            header += process.stdout.read(1)

        if header[:2] != b'\xCA\xFE':
            print('ERROR')
            print(header[:2])
            continue

        length = (header[3] << 8) | header[2] # depends on ENDIANNESS

        # Needs timeout
        payload = bytearray()
        while len(payload) < length:
            payload += process.stdout.read(1)

        # print(payload.decode(encoding="utf-8"))

        packet = Packet.Packet.GetRootAsPacket(payload, 0)
        status = status_code_to_string(packet.Status())
        native_status = packet.NativeStatus()
        native_class = packet.NativeClass()

        if packet.PayloadType() == Payload.Payload().GetMGMTInfo:
            getmgmtinfo = GetMGMTInfo.GetMGMTInfo()
            getmgmtinfo.Init(packet.Payload().Bytes, packet.Payload().Pos)
            version = getmgmtinfo.Version()
            revision = getmgmtinfo.Revision()

            print("GetMGMTInfo",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Version:", version, "Revision:", revision)
        elif packet.PayloadType() == Payload.Payload().StartScan:
            startscan = StartScan.StartScan()
            startscan.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = startscan.ControllerId()
            address_type = startscan.AddressType()

            print("StartScan",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller id:", controller_id, "Address type:", address_type)
        elif packet.PayloadType() == Payload.Payload().StopScan:
            stopscan = StopScan.StopScan()
            stopscan.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = stopscan.ControllerId()
            address_type = stopscan.AddressType()

            print("StopScan",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller id:", controller_id, "Address type:", address_type)
        elif packet.PayloadType() == Payload.Payload().AddDevice:
            add_device = AddDevice.AddDevice()
            add_device.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = add_device.ControllerId()
            address_type = add_device.AddressType()
            address = add_device.AddressAsNumpy()

            print("AddDevice",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller id:", controller_id, "Address type:", address_type, "Address:", address)

        elif packet.PayloadType() == Payload.Payload().RemoveDevice:
            remove_device = RemoveDevice.RemoveDevice()
            remove_device.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = remove_device.ControllerId()
            address_type = remove_device.AddressType()
            address = remove_device.AddressAsNumpy()

            print("RemoveDevice",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller id:", controller_id, "Address type:", address_type, "Address:", address)
        elif packet.PayloadType() == Payload.Payload().Discovering:
            discovering = Discovering.Discovering()
            discovering.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = discovering.ControllerId()
            address_type = discovering.AddressType()
            state = discovering.State()

            print("Discovering",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller id:", controller_id, "Address type:", address_type, "State:", state)

        elif packet.PayloadType() == Payload.Payload().DeviceFound:
            devicefound = DeviceFound.DeviceFound()
            devicefound.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = devicefound.ControllerId()
            address_type = devicefound.AddressType()
            address = devicefound.Address()
            rssi = devicefound.Rssi()
            flags = devicefound.FlagsAsNumpy()
            device_uuid = devicefound.Uuid()
            company_id = devicefound.CompanyId()
            device_name = devicefound.DeviceName()

            print("DeviceFound",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller id:", controller_id, "Address type:", address_type, "Address:", address,
                  "RSSI:", rssi, "Flags:", flags, "Device UUID:", device_uuid, "Company id:", company_id,
                  "Device name:", device_name)

        elif packet.PayloadType() == Payload.Payload().DeviceConnected:
            device_connected = DeviceConnected.DeviceConnected()
            device_connected.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = device_connected.ControllerId()
            address_type = device_connected.AddressType()
            address = device_connected.Address()
            flags = device_connected.FlagsAsNumpy()
            device_uuid = device_connected.Uuid()
            company_id = device_connected.CompanyId()
            device_name = device_connected.DeviceName()

            print("DeviceConnected",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller id:", controller_id, "Address type:", address_type, "Address:", address,
                  "Flags:", flags, "Device UUID:", device_uuid, "Company id:", company_id, "Device name:", device_name)

        elif packet.PayloadType() == Payload.Payload().DeviceDisconnected:
            device_disconnected = DeviceDisconnected.DeviceDisconnected()
            device_disconnected.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = device_disconnected.ControllerId()
            address_type = device_disconnected.AddressType()
            address = device_disconnected.Address()
            reason = device_disconnected.Reason()

            print("DeviceDisconnected",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller id:", controller_id, "Address type:", address_type, "Address:", address,
                  "Reason:", reason)

        elif packet.PayloadType() == Payload.Payload().Disconnect:
            disconnect = Disconnect.Disconnect()
            disconnect.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = disconnect.ControllerId()
            address_type = disconnect.AddressType()
            address = disconnect.AddressAsNumpy()

            print("Disconnect",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller id:", controller_id, "Address type:", address_type, "Address:", address)

        elif packet.PayloadType() == Payload.Payload().SetPowered:
            set_powered = SetPowered.SetPowered()
            set_powered.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = set_powered.ControllerId()
            state = set_powered.State()

            print("SetPowered",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller id:", controller_id, "State:", state)

        elif packet.PayloadType() == Payload.Payload().SetDiscoverable:
            set_discoverable = SetDiscoverable.SetDiscoverable()
            set_discoverable.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = set_discoverable.ControllerId()
            state = set_discoverable.State()
            timeout = set_discoverable.Timeout()

            print("SetDiscoverable",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller id:", controller_id, "State:", state, "Timeout:", timeout)

        elif packet.PayloadType() == Payload.Payload().SetConnectable:
            set_connectable = SetConnectable.SetConnectable()
            set_connectable.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = set_connectable.ControllerId()
            state = set_connectable.State()

            print("SetConnectable",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller id:", controller_id, "State:", state)

        elif packet.PayloadType() == Payload.Payload().GetControllersList:
            controllers_list = GetControllersList.GetControllersList()
            controllers_list.Init(packet.Payload().Bytes, packet.Payload().Pos)
            num_controllers = controllers_list.NumControllers()
            controllers = controllers_list.ControllersAsNumpy()

            print("GetControllersList",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Num controllers:", num_controllers, "Controllers ID:", controllers)

        elif packet.PayloadType() == Payload.Payload().ControllerAdded:
            controller_added = ControllerAdded.ControllerAdded()
            controller_added.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = controller_added.ControllerId()

            print("ControllerAdded",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id)

        elif packet.PayloadType() == Payload.Payload().ControllerRemoved:
            controller_removed = ControllerRemoved.ControllerRemoved()
            controller_removed.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = controller_removed.ControllerId()

            print("ControllerRemoved",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id)

        elif packet.PayloadType() == Payload.Payload().GetControllerInfo:
            controller_info = GetControllerInfo.GetControllerInfo()
            controller_info.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = controller_info.ControllerId()
            address = controller_info.Address()
            bt_version = controller_info.BtVersion()
            powered = controller_info.Powered()
            connectable = controller_info.Connectable()
            discoverable = controller_info.Discoverable()
            low_energy = controller_info.LowEnergy()
            name = controller_info.Name()

            print("GetControllerInfo",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address:", address, "Bluetooth version:", bt_version,
                  "Powered:", powered, "Connectable:", connectable, "Discoverable:", discoverable,
                  "LE supported:", low_energy, "Name:", name)


        elif packet.PayloadType() == Payload.Payload().BaBLEError:
            error = BaBLEError.BaBLEError()
            error.Init(packet.Payload().Bytes, packet.Payload().Pos)
            message = error.Message()

            print("BaBLEError",
                  "Status:", status, "Native class: ", native_class, "Native status:", native_status,
                  "Message:", message)
        else:
            print('NOPE...')
except KeyboardInterrupt:
    print("Stopping python interface...")

while process.poll() is None:
    print("Waiting for subprocess to stop")
    time.sleep(0.1)

print("Python interface terminated.")
