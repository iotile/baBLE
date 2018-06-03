import subprocess
import time
import flatbuffers

from bable_interface.BaBLE import Packet, Payload, GetMGMTInfo, StartScan, StopScan, Discovering, DeviceFound, BaBLEError, StatusCode, \
    AddDevice, DeviceConnected, RemoveDevice, Disconnect, DeviceDisconnected, SetPowered, \
    SetDiscoverable, SetConnectable, GetControllersList, ControllerAdded, ControllerRemoved, \
    GetControllerInfo, GetConnectedDevices, GetControllersIds, Read, Write, NotificationReceived, \
    ProbeServices, ProbeCharacteristics, WriteWithoutResponse, Ready, Exit, DeviceAdded, DeviceRemoved, ErrorResponse


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


process = subprocess.Popen(["../../platforms/linux/build/debug/baBLE_linux", "--logging", "debug"],
                           stdout=subprocess.PIPE, stdin=subprocess.PIPE,
                           bufsize=0,
                           universal_newlines=False)

#### ASCII

# process.stdin.write(b'\xCA\xFE' + len("1").to_bytes(2, byteorder='little') + bytes("2,0,7", encoding="utf-8"))
# process.stdin.write(bytes("2,0", encoding="utf-8"))
# time.sleep(2)
# process.stdin.write("5,0")


#### Flatbuffers

## StartScan
def fb_start_scan(uuid, controller_id):
    builder = flatbuffers.Builder(0)

    StartScan.StartScanStart(builder)
    StartScan.StartScanAddActiveScan(builder, True)
    payload = StartScan.StartScanEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.StartScan, controller_id)


## Stop scan
def fb_stop_scan(uuid, controller_id):
    builder = flatbuffers.Builder(0)
    StopScan.StopScanStart(builder)
    payload = StopScan.StopScanEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.StopScan, controller_id)


## AddDevice
def fb_add_device(uuid, controller_id, address_device):
    builder = flatbuffers.Builder(0)
    address = builder.CreateString(address_device)

    AddDevice.AddDeviceStart(builder)
    AddDevice.AddDeviceAddAddress(builder, address)
    AddDevice.AddDeviceAddAddressType(builder, 2)
    payload = AddDevice.AddDeviceEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.AddDevice, controller_id)


## RemoveDevice
def fb_remove_device(uuid, controller_id, address_device):
    builder = flatbuffers.Builder(0)
    address = builder.CreateString(address_device)

    RemoveDevice.RemoveDeviceStart(builder)
    RemoveDevice.RemoveDeviceAddAddress(builder, address)
    RemoveDevice.RemoveDeviceAddAddressType(builder, 2)
    payload = RemoveDevice.RemoveDeviceEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.RemoveDevice, controller_id)


## Disconnect
def fb_disconnect(uuid, controller_id, address_device):
    builder = flatbuffers.Builder(0)
    address = builder.CreateString(address_device)

    Disconnect.DisconnectStart(builder)
    Disconnect.DisconnectAddAddress(builder, address)
    Disconnect.DisconnectAddAddressType(builder, 2)
    payload = Disconnect.DisconnectEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.Disconnect, controller_id)


## SetPowered
def fb_set_powered(uuid, controller_id, state):
    builder = flatbuffers.Builder(0)
    SetPowered.SetPoweredStart(builder)
    SetPowered.SetPoweredAddState(builder, state)
    payload = SetPowered.SetPoweredEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.SetPowered, controller_id)


## SetDiscoverable
def fb_set_discoverable(uuid, controller_id, state):
    builder = flatbuffers.Builder(0)
    SetDiscoverable.SetDiscoverableStart(builder)
    SetDiscoverable.SetDiscoverableAddState(builder, state)
    payload = SetDiscoverable.SetDiscoverableEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.SetDiscoverable, controller_id)


## SetConnectable
def fb_set_connectable(uuid, controller_id, state):
    builder = flatbuffers.Builder(0)
    SetConnectable.SetConnectableStart(builder)
    SetConnectable.SetConnectableAddState(builder, state)
    payload = SetConnectable.SetConnectableEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.SetConnectable, controller_id)


## GetControllersList
def fb_get_controllers_list(uuid):
    builder = flatbuffers.Builder(0)
    GetControllersList.GetControllersListStart(builder)
    payload = GetControllersList.GetControllersListEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.GetControllersList)


## GetControllersIds
def fb_get_controllers_ids(uuid):
    builder = flatbuffers.Builder(0)
    GetControllersIds.GetControllersIdsStart(builder)
    payload = GetControllersIds.GetControllersIdsEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.GetControllersIds)


## GetControllerInfo
def fb_get_controller_info(uuid, controller_id):
    builder = flatbuffers.Builder(0)
    GetControllerInfo.GetControllerInfoStart(builder)
    payload = GetControllerInfo.GetControllerInfoEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.GetControllerInfo, controller_id)


## GetControllerInfo
def fb_get_mgmt_info(uuid):
    builder = flatbuffers.Builder(0)
    GetMGMTInfo.GetMGMTInfoStart(builder)
    payload = GetMGMTInfo.GetMGMTInfoEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.GetMGMTInfo)


## GetConnectedDevices
def fb_get_connected_devices(uuid, controller_id):
    builder = flatbuffers.Builder(0)
    GetConnectedDevices.GetConnectedDevicesStart(builder)
    payload = GetConnectedDevices.GetConnectedDevicesEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.GetConnectedDevices, controller_id)


## Read
def fb_read(uuid, controller_id, connection_handle, attribute_handle):
    builder = flatbuffers.Builder(0)
    Read.ReadStart(builder)
    Read.ReadAddConnectionHandle(builder, connection_handle)
    Read.ReadAddAttributeHandle(builder, attribute_handle)
    payload = Read.ReadEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.Read, controller_id)


## Write
def fb_write(uuid, controller_id, connection_handle, attribute_handle, value):
    builder = flatbuffers.Builder(0)

    data = bytes(value, 'utf-8')
    Write.WriteStartValueVector(builder, len(data))
    for element in reversed(data):
        builder.PrependByte(element)
    data_to_write = builder.EndVector(len(data))

    Write.WriteStart(builder)
    Write.WriteAddConnectionHandle(builder, connection_handle)
    Write.WriteAddAttributeHandle(builder, attribute_handle)
    Write.WriteAddValue(builder, data_to_write)
    payload = Write.WriteEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.Write, controller_id)


## WriteWithoutResponse
def fb_write_without_response(uuid, controller_id, connection_handle, attribute_handle, value):
    builder = flatbuffers.Builder(0)

    data = bytes(value, 'utf-8')
    Write.WriteStartValueVector(builder, len(data))
    for element in reversed(data):
        builder.PrependByte(element)
    data_to_write = builder.EndVector(len(data))

    Write.WriteStart(builder)
    Write.WriteAddConnectionHandle(builder, connection_handle)
    Write.WriteAddAttributeHandle(builder, attribute_handle)
    Write.WriteAddValue(builder, data_to_write)
    payload = Write.WriteEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.WriteWithoutResponse, controller_id)


## ProbeServices
def fb_probe_services(uuid, controller_id, connection_handle):
    builder = flatbuffers.Builder(0)
    ProbeServices.ProbeServicesStart(builder)
    ProbeServices.ProbeServicesAddConnectionHandle(builder, connection_handle)
    payload = ProbeServices.ProbeServicesEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.ProbeServices, controller_id)


## ProbeCharacteristics
def fb_probe_characteristics(uuid, controller_id, connection_handle):
    builder = flatbuffers.Builder(0)
    ProbeCharacteristics.ProbeCharacteristicsStart(builder)
    ProbeCharacteristics.ProbeCharacteristicsAddConnectionHandle(builder, connection_handle)
    payload = ProbeCharacteristics.ProbeCharacteristicsEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.ProbeCharacteristics, controller_id)


## Exit
def fb_exit():
    builder = flatbuffers.Builder(0)
    Exit.ExitStart(builder)
    payload = Exit.ExitEnd(builder)

    return build_packet(builder, "", payload, Payload.Payload.Exit)


def build_packet(builder, uuid, payload, payload_type, controller_id = None):
    uuid_request = builder.CreateString(uuid)

    Packet.PacketStart(builder)
    Packet.PacketAddUuid(builder, uuid_request)
    if controller_id is not None:
        Packet.PacketAddControllerId(builder, controller_id)
    Packet.PacketAddPayloadType(builder, payload_type)
    Packet.PacketAddPayload(builder, payload)
    packet = Packet.PacketEnd(builder)

    builder.Finish(packet)

    buf = builder.Output()
    buf = b'\xCA\xFE' + len(buf).to_bytes(2, byteorder='little') + buf
    return buf


address_device = [0xC4, 0xF0, 0xA5, 0xE6, 0x8A, 0x91]
address_device_str = "C4:F0:A5:E6:8A:91"

header_length = 4
exitted = False

try:
    while True:
        if process.poll() is not None:
            print("Subprocess terminated.")
            break

        if exitted:
            break

        header = bytearray()
        while len(header) < header_length:
            header += process.stdout.read(1)

        if header[:2] != b'\xCA\xFE':
            print('ERROR')
            print(header[:2])
            continue

        length = (header[3] << 8) | header[2]  # depends on ENDIANNESS

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

            process.stdin.write(fb_get_connected_devices("mgmt", 0))
        elif packet.PayloadType() == Payload.Payload().StartScan:
            startscan = StartScan.StartScan()
            startscan.Init(packet.Payload().Bytes, packet.Payload().Pos)
            active_scan = startscan.ActiveScan()

            print("StartScan",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Active scan:", active_scan)
        elif packet.PayloadType() == Payload.Payload().StopScan:
            stopscan = StopScan.StopScan()
            stopscan.Init(packet.Payload().Bytes, packet.Payload().Pos)

            print("StopScan",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id)
        elif packet.PayloadType() == Payload.Payload().AddDevice:
            add_device = AddDevice.AddDevice()
            add_device.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address_type = add_device.AddressType()
            address = add_device.Address()

            print("AddDevice",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address type:", address_type, "Address:", address)
        elif packet.PayloadType() == Payload.Payload().RemoveDevice:
            remove_device = RemoveDevice.RemoveDevice()
            remove_device.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address_type = remove_device.AddressType()
            address = remove_device.Address()

            print("RemoveDevice",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address type:", address_type, "Address:", address)

            process.stdin.write(fb_disconnect("disconnect", 0, address_device_str))
        elif packet.PayloadType() == Payload.Payload().Discovering:
            discovering = Discovering.Discovering()
            discovering.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address_type = discovering.AddressType()
            state = discovering.State()

            print("Discovering",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address type:", address_type, "State:", state)

            if state is False:
                process.stdin.write(fb_exit())

        elif packet.PayloadType() == Payload.Payload().DeviceFound:
            devicefound = DeviceFound.DeviceFound()
            devicefound.Init(packet.Payload().Bytes, packet.Payload().Pos)
            type = devicefound.Type()
            address_type = devicefound.AddressType()
            address = devicefound.Address()
            rssi = devicefound.Rssi()
            device_uuid = devicefound.Uuid()
            company_id = devicefound.CompanyId()
            manufacturer_data = devicefound.ManufacturerDataAsNumpy()
            device_name = devicefound.DeviceName()

            print("DeviceFound",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address type:", address_type, "Address:", address,
                  "RSSI:", rssi, "Type:", type, "Device UUID:", device_uuid, "Company id:", company_id,
                  "Manufacturer data:", manufacturer_data, "Device name:", device_name)

            # process.stdin.write(fb_stop_scan("0002", 0))

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

            # process.stdin.write(fb_write_without_response("0002", 0, 0x0040, 0x0003, "Alex"))
            # time.sleep(2)
            # process.stdin.write(fb_read("0002", 0, 0x0040, 0x0003))

            process.stdin.write(fb_probe_characteristics("12356789", 0, 0x0040))
            # process.stdin.write(fb_probe_services("12356789", 0, 0x0040))
            # process.stdin.write(fb_remove_device("remove", 0, address_device_str))
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

            process.stdin.write(fb_exit())

        elif packet.PayloadType() == Payload.Payload().Disconnect:
            disconnect = Disconnect.Disconnect()
            disconnect.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address_type = disconnect.AddressType()
            address = disconnect.Address()

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

            # process.stdin.write(fb_get_controller_info("test", 0))
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
        elif packet.PayloadType() == Payload.Payload().DeviceAdded:
            device_added = DeviceAdded.DeviceAdded()
            device_added.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address = device_added.Address()
            address_type = device_added.AddressType()

            print("DeviceAdded",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address:", address, "Address type:", address_type)
        elif packet.PayloadType() == Payload.Payload().DeviceRemoved:
            device_removed = DeviceRemoved.DeviceRemoved()
            device_removed.Init(packet.Payload().Bytes, packet.Payload().Pos)
            address = device_removed.Address()
            address_type = device_removed.AddressType()

            print("DeviceRemoved",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Address:", address, "Address type:", address_type)
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
                  "Controller ID:", controller_id, "Connection handle:", connection_handle,
                  "Attribute handle:", attribute_handle, "Data:", data_read)

            process.stdin.write(fb_remove_device("0003", 0, address_device_str))
            process.stdin.write(fb_disconnect("0004", 0, address_device_str))
        elif packet.PayloadType() == Payload.Payload().Write:
            write = Write.Write()
            write.Init(packet.Payload().Bytes, packet.Payload().Pos)
            connection_handle = write.ConnectionHandle()
            attribute_handle = write.AttributeHandle()
            data_written = write.ValueAsNumpy()

            print("Write",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Connection handle:", connection_handle,
                  "Attribute handle:", attribute_handle, "Data:", data_written)

        elif packet.PayloadType() == Payload.Payload().WriteWithoutResponse:
            write_without_response = WriteWithoutResponse.WriteWithoutResponse()
            write_without_response.Init(packet.Payload().Bytes, packet.Payload().Pos)
            connection_handle = write_without_response.ConnectionHandle()
            attribute_handle = write_without_response.AttributeHandle()
            data_written = write_without_response.ValueAsNumpy()

            print("WriteWithoutResponse",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Connection handle:", connection_handle,
                  "Attribute handle:", attribute_handle, "Data:", data_written)
        elif packet.PayloadType() == Payload.Payload().NotificationReceived:
            notification_received = NotificationReceived.NotificationReceived()
            notification_received.Init(packet.Payload().Bytes, packet.Payload().Pos)
            connection_handle = notification_received.ConnectionHandle()
            attribute_handle = notification_received.AttributeHandle()
            data_received = notification_received.ValueAsNumpy()

            print("NotificationReceived",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Connection handle:", connection_handle,
                  "Attribute handle:", attribute_handle, "Data:", data_received)
        elif packet.PayloadType() == Payload.Payload().ProbeServices:
            probe_services = ProbeServices.ProbeServices()
            probe_services.Init(packet.Payload().Bytes, packet.Payload().Pos)
            num_services = probe_services.ServicesLength()
            services = []

            for i in range(num_services):
                services.append(probe_services.Services(i))

            print("ProbeServices",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Num services:", num_services)

            for service in services:
                print("\tHandle:", service.Handle(), "Group end handle:", service.GroupEndHandle(),
                      "UUID:", service.Uuid())

            process.stdin.write(fb_remove_device("0003", 0, address_device_str))
            process.stdin.write(fb_disconnect("0004", 0, address_device_str))
        elif packet.PayloadType() == Payload.Payload().ProbeCharacteristics:
            probe_characteristics = ProbeCharacteristics.ProbeCharacteristics()
            probe_characteristics.Init(packet.Payload().Bytes, packet.Payload().Pos)
            num_characteristics = probe_characteristics.CharacteristicsLength()
            characteristics = []

            for i in range(num_characteristics):
                characteristics.append(probe_characteristics.Characteristics(i))

            print("ProbeCharacteristics",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Num characteristics:", num_characteristics)

            for characteristic in characteristics:
                print("\tHandle:", characteristic.Handle(), "Value handle:", characteristic.ValueHandle(),
                      "UUID:", characteristic.Uuid(), "Indicate:", characteristic.Indicate(),
                      "Notify:", characteristic.Notify(), "Read:", characteristic.Read(),
                      "Write:", characteristic.Write(), "Broadcast:", characteristic.Broadcast())

            process.stdin.write(fb_remove_device("0003", 0, address_device_str))
            process.stdin.write(fb_disconnect("0004", 0, address_device_str))
        elif packet.PayloadType() == Payload.Payload().Ready:
            ready = Ready.Ready()
            ready.Init(packet.Payload().Bytes, packet.Payload().Pos)

            print("ReadyPacket")

            # process.stdin.write(fb_add_device("0001", 0, address_device_str))
            # process.stdin.write(fb_probe_characteristics("0001", 0, 0x0040))
            process.stdin.write(fb_start_scan("0001", 0))
            time.sleep(3)
            process.stdin.write(fb_stop_scan("0002", 0))
            # process.stdin.write(fb_get_controllers_ids("0001"))
            # process.stdin.write(fb_set_powered("0001", 0, True))
            # process.stdin.write(fb_get_controllers_list("list"))

        elif packet.PayloadType() == Payload.Payload().BaBLEError:
            error = BaBLEError.BaBLEError()
            error.Init(packet.Payload().Bytes, packet.Payload().Pos)
            message = error.Message()

            print("BaBLEError",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Message:", message)

        elif packet.PayloadType() == Payload.Payload().ErrorResponse:
            error = ErrorResponse.ErrorResponse()
            error.Init(packet.Payload().Bytes, packet.Payload().Pos)
            opcode = error.Opcode()
            handle = error.Handle()

            print("ErrorResponse",
                  "UUID:", uuid,
                  "Status:", status, "Native class:", native_class, "Native status:", native_status,
                  "Controller ID:", controller_id, "Opcode:", opcode, "Handle:", handle)

        elif packet.PayloadType() == Payload.Payload().Exit:
            print("Exit")
            exitted = True

        else:
            print('NOPE...')
except KeyboardInterrupt:
    print("Stopping python interface...")

while process.poll() is None:
    print("Waiting for subprocess to stop")
    time.sleep(0.1)

print("Python interface terminated.")
