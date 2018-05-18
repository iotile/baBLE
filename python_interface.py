import subprocess
import sys
import time
import flatbuffers

from Schemas import Packet, Payload, GetMGMTInfo, StartScan, StopScan, Discovering, DeviceFound, BaBLEError, StatusCode,\
                    AddDevice, DeviceConnected, RemoveDevice, Disconnect, DeviceDisconnected, SetPowered, SetDiscoverable,\
                    SetConnectable, GetControllersList, ControllerAdded, ControllerRemoved, GetControllerInfo, \
                    GetConnectedDevices, GetControllersIds, Read, Write

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
# StartScan.StartScanAddAddressType(builder, 0x07)
# payload = StartScan.StartScanEnd(builder)

## Stop scan
# StopScan.StopScanStart(builder)
# StopScan.StopScanAddAddressType(builder, 0x07)
# payload = StopScan.StopScanEnd(builder)

## AddDevice
AddDevice.AddDeviceStartAddressVector(builder, 6)
address_list = [0xC4, 0xF0, 0xA5, 0xE6, 0x8A, 0x91]
for element in address_list:
    builder.PrependByte(element)
address = builder.EndVector(6)

AddDevice.AddDeviceStart(builder)
AddDevice.AddDeviceAddAddress(builder, address)
AddDevice.AddDeviceAddAddressType(builder, 2)
payload = AddDevice.AddDeviceEnd(builder)

## RemoveDevice
# RemoveDevice.RemoveDeviceStartAddressVector(builder, 6)
# address_list = [0xC4, 0xF0, 0xA5, 0xE6, 0x8A, 0x91]
# for element in address_list:
#     builder.PrependByte(element)
# address = builder.EndVector(6)
#
# RemoveDevice.RemoveDeviceStart(builder)
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
# Disconnect.DisconnectAddAddress(builder, address)
# Disconnect.DisconnectAddAddressType(builder, 2)
# payload = Disconnect.DisconnectEnd(builder)

## SetPowered
# SetPowered.SetPoweredStart(builder)
# SetPowered.SetPoweredAddState(builder, False)
# payload = SetPowered.SetPoweredEnd(builder)

## SetDiscoverable
# SetDiscoverable.SetDiscoverableStart(builder)
# SetDiscoverable.SetDiscoverableAddState(builder, False)
# payload = SetDiscoverable.SetDiscoverableEnd(builder)

## SetConnectable
# SetConnectable.SetConnectableStart(builder)
# SetConnectable.SetConnectableAddState(builder, False)
# payload = SetConnectable.SetConnectableEnd(builder)

## GetControllersList
# GetControllersList.GetControllersListStart(builder)
# payload = GetControllersList.GetControllersListEnd(builder)

## GetControllersIds
# GetControllersIds.GetControllersIdsStart(builder)
# payload = GetControllersIds.GetControllersIdsEnd(builder)

## GetControllerInfo
# GetControllerInfo.GetControllerInfoStart(builder)
# payload = GetControllerInfo.GetControllerInfoEnd(builder)

## GetConnectedDevices
# GetConnectedDevices.GetConnectedDevicesStart(builder)
# payload = GetConnectedDevices.GetConnectedDevicesEnd(builder)

## Read
# Read.ReadStart(builder)
# Read.ReadAddConnectionHandle(builder, 64)
# Read.ReadAddAttributeHandle(builder, 0x0003)
# payload = Read.ReadEnd(builder)

uuid_request = builder.CreateString("123456")
Packet.PacketStart(builder)
Packet.PacketAddUuid(builder, uuid_request)
Packet.PacketAddControllerId(builder, 0)
Packet.PacketAddPayloadType(builder, Payload.Payload().AddDevice)
Packet.PacketAddPayload(builder, payload)
packet = Packet.PacketEnd(builder)

builder.Finish(packet)

buf = builder.Output()
buf = b'\xCA\xFE' + len(buf).to_bytes(2, byteorder='little') + buf


builder2 = flatbuffers.Builder(0)
data = bytes('IOTile', 'utf-8')
Write.WriteStartValueVector(builder2, len(data))
for element in reversed(data):
    builder2.PrependByte(element)
data_to_write = builder2.EndVector(len(data))

Write.WriteStart(builder2)
Write.WriteAddConnectionHandle(builder2, 64)
Write.WriteAddAttributeHandle(builder2, 0x0003)
Write.WriteAddValue(builder2, data_to_write)
payload2 = Write.WriteEnd(builder2)

uuid_request2 = builder2.CreateString("654321")
Packet.PacketStart(builder2)
Packet.PacketAddUuid(builder2, uuid_request2)
Packet.PacketAddControllerId(builder2, 0)
Packet.PacketAddPayloadType(builder2, Payload.Payload().Write)
Packet.PacketAddPayload(builder2, payload2)
packet2 = Packet.PacketEnd(builder2)

builder2.Finish(packet2)

buf2 = builder2.Output()
buf2 = b'\xCA\xFE' + len(buf2).to_bytes(2, byteorder='little') + buf2

time.sleep(2)
process.stdin.write(buf)
time.sleep(8)
process.stdin.write(buf2)

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
        controller_id = packet.ControllerId()
        uuid = packet.Uuid()
        status = status_code_to_string(packet.Status())
        native_status = packet.NativeStatus()
        native_class = packet.NativeClass()

        if packet.PayloadType() == Payload.Payload().GetMGMTInfo:
            getmgmtinfo = GetMGMTInfo.GetMGMTInfo()
            getmgmtinfo.Init(packet.Payload().Bytes, packet.Payload().Pos)
            version = getmgmtinfo.Version()
            revision = getmgmtinfo.Revision()

            print("GetMGMTInfo",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Version:", version, "Revision:", revision)
        elif packet.PayloadType() == Payload.Payload().StartScan:
            startscan = StartScan.StartScan()
            startscan.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address_type = startscan.AddressType()

            print("StartScan",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address type:", address_type)
        elif packet.PayloadType() == Payload.Payload().StopScan:
            stopscan = StopScan.StopScan()
            stopscan.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address_type = stopscan.AddressType()

            print("StopScan",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address type:", address_type)
        elif packet.PayloadType() == Payload.Payload().AddDevice:
            add_device = AddDevice.AddDevice()
            add_device.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address_type = add_device.AddressType()
            address = add_device.AddressAsNumpy()

            print("AddDevice",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address type:", address_type, "Address:", address)

        elif packet.PayloadType() == Payload.Payload().RemoveDevice:
            remove_device = RemoveDevice.RemoveDevice()
            remove_device.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address_type = remove_device.AddressType()
            address = remove_device.AddressAsNumpy()

            print("RemoveDevice",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address type:", address_type, "Address:", address)
        elif packet.PayloadType() == Payload.Payload().Discovering:
            discovering = Discovering.Discovering()
            discovering.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address_type = discovering.AddressType()
            state = discovering.State()

            print("Discovering",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address type:", address_type, "State:", state)

        elif packet.PayloadType() == Payload.Payload().DeviceFound:
            devicefound = DeviceFound.DeviceFound()
            devicefound.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address_type = devicefound.AddressType()
            address = devicefound.Address()
            rssi = devicefound.Rssi()
            flags = devicefound.FlagsAsNumpy()
            device_uuid = devicefound.Uuid()
            company_id = devicefound.CompanyId()
            device_name = devicefound.DeviceName()

            print("DeviceFound",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address type:", address_type, "Address:", address,
                  "RSSI:", rssi, "Flags:", flags, "Device UUID:", device_uuid, "Company id:", company_id,
                  "Device name:", device_name)

        elif packet.PayloadType() == Payload.Payload().DeviceConnected:
            device_connected = DeviceConnected.DeviceConnected()
            device_connected.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address_type = device_connected.AddressType()
            address = device_connected.Address()
            connection_handle = device_connected.ConnectionHandle()
            flags = device_connected.FlagsAsNumpy()
            device_uuid = device_connected.Uuid()
            company_id = device_connected.CompanyId()
            device_name = device_connected.DeviceName()

            print("DeviceConnected",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Connection handle:", connection_handle,
                  "Controller ID:", controller_id, "Address type:", address_type, "Address:", address,
                  "Flags:", flags, "Device UUID:", device_uuid, "Company id:", company_id, "Device name:", device_name)

        elif packet.PayloadType() == Payload.Payload().DeviceDisconnected:
            device_disconnected = DeviceDisconnected.DeviceDisconnected()
            device_disconnected.Init(packet.Payload().Bytes, packet.Payload().Pos)
            connection_handle = device_disconnected.ConnectionHandle()
            address_type = device_disconnected.AddressType()
            address = device_disconnected.Address()
            reason = device_disconnected.Reason()

            print("DeviceDisconnected",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Connection handle:", connection_handle,
                  "Controller ID:", controller_id, "Address type:", address_type, "Address:", address,
                  "Reason:", reason)

        elif packet.PayloadType() == Payload.Payload().Disconnect:
            disconnect = Disconnect.Disconnect()
            disconnect.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address_type = disconnect.AddressType()
            address = disconnect.AddressAsNumpy()

            print("Disconnect",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address type:", address_type, "Address:", address)

        elif packet.PayloadType() == Payload.Payload().SetPowered:
            set_powered = SetPowered.SetPowered()
            set_powered.Init(packet.Payload().Bytes, packet.Payload().Pos)
            state = set_powered.State()

            print("SetPowered",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "State:", state)

        elif packet.PayloadType() == Payload.Payload().SetDiscoverable:
            set_discoverable = SetDiscoverable.SetDiscoverable()
            set_discoverable.Init(packet.Payload().Bytes, packet.Payload().Pos)
            state = set_discoverable.State()
            timeout = set_discoverable.Timeout()

            print("SetDiscoverable",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "State:", state, "Timeout:", timeout)

        elif packet.PayloadType() == Payload.Payload().SetConnectable:
            set_connectable = SetConnectable.SetConnectable()
            set_connectable.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = set_connectable.ControllerId()
            state = set_connectable.State()

            print("SetConnectable",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "State:", state)

        elif packet.PayloadType() == Payload.Payload().GetControllersList:
            controllers_list = GetControllersList.GetControllersList()
            controllers_list.Init(packet.Payload().Bytes, packet.Payload().Pos)
            num_controllers = controllers_list.ControllersLength()
            controllers = []

            for i in range(num_controllers):
                controllers.append(controllers_list.Controllers(i))

            print("GetControllersList",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Num controllers:", num_controllers)

            for controller in controllers:
                print("\tController ID:", controller.Id(), "Address:", controller.Address(),
                      "Bluetooth version:", controller.BtVersion(), "Powered:", controller.Powered(),
                      "Connectable:", controller.Connectable(), "Discoverable:", controller.Discoverable(),
                      "LE supported:", controller.LowEnergy(), "Name:", controller.Name())

        elif packet.PayloadType() == Payload.Payload().GetControllersIds:
            controllers_ids = GetControllersIds.GetControllersIds()
            controllers_ids.Init(packet.Payload().Bytes, packet.Payload().Pos)
            num_controllers = controllers_ids.ControllersIdsLength()
            ids = controllers_ids.ControllersIdsAsNumpy()

            print("GetControllersIds",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controllers IDs:", ids, "Num controllers:", num_controllers)

        elif packet.PayloadType() == Payload.Payload().ControllerAdded:
            controller_added = ControllerAdded.ControllerAdded()
            controller_added.Init(packet.Payload().Bytes, packet.Payload().Pos)

            print("ControllerAdded",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id)

        elif packet.PayloadType() == Payload.Payload().ControllerRemoved:
            controller_removed = ControllerRemoved.ControllerRemoved()
            controller_removed.Init(packet.Payload().Bytes, packet.Payload().Pos)

            print("ControllerRemoved",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id)

        elif packet.PayloadType() == Payload.Payload().GetControllerInfo:
            controller_info = GetControllerInfo.GetControllerInfo()
            controller_info.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller = controller_info.ControllerInfo()

            print("GetControllerInfo",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID: ", controller.Id(), "Address:", controller.Address(),
                  "Bluetooth version:", controller.BtVersion(), "Powered:", controller.Powered(),
                  "Connectable:", controller.Connectable(), "Discoverable:", controller.Discoverable(),
                  "LE supported:", controller.LowEnergy(), "Name:", controller.Name())

        elif packet.PayloadType() == Payload.Payload().GetConnectedDevices:
            connected_devices = GetConnectedDevices.GetConnectedDevices()
            connected_devices.Init(packet.Payload().Bytes, packet.Payload().Pos)
            num_connections = connected_devices.DevicesLength()
            devices = []
            for i in range(num_connections):
                devices.append(connected_devices.Devices(i))

            print("GetConnectedDevices",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Num connections:", num_connections, "Device addresses:", devices)

        elif packet.PayloadType() == Payload.Payload().Read:
            read = Read.Read()
            read.Init(packet.Payload().Bytes, packet.Payload().Pos)
            connection_handle = read.ConnectionHandle()
            attribute_handle = read.AttributeHandle()
            data_read = read.ValueAsNumpy()

            print("Read",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Connection handle:", connection_handle, "Attribute handle:", attribute_handle,
                  "Data:", data_read)

        elif packet.PayloadType() == Payload.Payload().Write:
            write = Write.Write()
            write.Init(packet.Payload().Bytes, packet.Payload().Pos)
            connection_handle = write.ConnectionHandle()
            attribute_handle = write.AttributeHandle()
            data_written = write.ValueAsNumpy()

            print("Write",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Connection handle:", connection_handle, "Attribute handle:", attribute_handle,
                  "Data:", data_written)

        elif packet.PayloadType() == Payload.Payload().BaBLEError:
            error = BaBLEError.BaBLEError()
            error.Init(packet.Payload().Bytes, packet.Payload().Pos)
            message = error.Message()

            print("BaBLEError",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Message:", message)
        else:
            print('NOPE...')
except KeyboardInterrupt:
    print("Stopping python interface...")

while process.poll() is None:
    print("Waiting for subprocess to stop")
    time.sleep(0.1)

print("Python interface terminated.")
