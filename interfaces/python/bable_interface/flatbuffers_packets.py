import flatbuffers
import struct
import sys

from .BaBLE import Packet, Payload, Connect, StartScan, Exit, StopScan, ProbeServices, \
    ProbeCharacteristics, Disconnect

PYTHON_2 = sys.version_info[0] < 3

if PYTHON_2:
    def to_bytes(value, length, byteorder='big'):
        struct_str = '>' if byteorder == 'big' else '<'
        if value < 0:
            raise ValueError("Can't convert negative values to bytes.")

        if length == 1:
            struct_str += 'B'
        elif length == 2:
            struct_str += 'H'
        elif length == 4:
            struct_str += 'I'
        elif length == 8:
            struct_str += 'Q'
        else:
            raise ValueError("Invalid length.")

        return struct.pack(struct_str, value)

else:
    def to_bytes(value, length, byteorder='big'):
        return value.to_bytes(length, byteorder=byteorder)


def build_packet(builder, uuid, payload, payload_type, controller_id=None):
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
    buf = b'\xCA\xFE' + to_bytes(len(buf), 2, byteorder='little') + buf
    return buf


## Flatbuffers
def fb_exit():
    builder = flatbuffers.Builder(0)
    Exit.ExitStart(builder)
    payload = Exit.ExitEnd(builder)

    return build_packet(builder, "", payload, Payload.Payload.Exit)


def fb_start_scan(uuid, controller_id, active_scan):
    builder = flatbuffers.Builder(0)

    StartScan.StartScanStart(builder)
    StartScan.StartScanAddActiveScan(builder, active_scan)
    payload = StartScan.StartScanEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.StartScan, controller_id)


def fb_stop_scan(uuid, controller_id):
    builder = flatbuffers.Builder(0)
    StopScan.StopScanStart(builder)
    payload = StopScan.StopScanEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.StopScan, controller_id)


def fb_connect(uuid, controller_id, address_device, address_type):
    builder = flatbuffers.Builder(0)
    address = builder.CreateString(address_device)

    Connect.ConnectStart(builder)
    Connect.ConnectAddAddress(builder, address)
    Connect.ConnectAddAddressType(builder, 0 if address_type == "public" else 1)
    payload = Connect.ConnectEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.Connect, controller_id)


def fb_probe_services(uuid, controller_id, connection_handle):
    builder = flatbuffers.Builder(0)
    ProbeServices.ProbeServicesStart(builder)
    ProbeServices.ProbeServicesAddConnectionHandle(builder, connection_handle)
    payload = ProbeServices.ProbeServicesEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.ProbeServices, controller_id)


def fb_probe_characteristics(uuid, controller_id, connection_handle):
    builder = flatbuffers.Builder(0)
    ProbeCharacteristics.ProbeCharacteristicsStart(builder)
    ProbeCharacteristics.ProbeCharacteristicsAddConnectionHandle(builder, connection_handle)
    payload = ProbeCharacteristics.ProbeCharacteristicsEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.ProbeCharacteristics, controller_id)


def fb_disconnect(uuid, controller_id, connection_handle):
    builder = flatbuffers.Builder(0)

    Disconnect.DisconnectStart(builder)
    Disconnect.DisconnectAddConnectionHandle(builder, connection_handle)
    payload = Disconnect.DisconnectEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.Disconnect, controller_id)
