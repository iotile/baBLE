import subprocess
import sys
import time
import flatbuffers

from Schemas import Packet, Payload, GetMGMTInfo, StartScan, StopScan, Discovering, DeviceFound

process = subprocess.Popen(["./build/debug/baBLE_linux"],
                           stdout=subprocess.PIPE, stdin=subprocess.PIPE,
                           bufsize=0,
                           universal_newlines=False)

## ASCII
# process.stdin.write(b'\xCA\xFE' + len("1").to_bytes(2, byteorder='little') + bytes("2,0,7", encoding="utf-8"))
# process.stdin.write(bytes("2,0", encoding="utf-8"))
# time.sleep(2)
# process.stdin.write("5,0")

## Flatbuffers
builder = flatbuffers.Builder(0)
# GetMGMTInfo.GetMGMTInfoStart(builder)
# payload = GetMGMTInfo.GetMGMTInfoEnd(builder)

StartScan.StartScanStart(builder)
StartScan.StartScanAddControllerId(builder, 0)
StartScan.StartScanAddAddressType(builder, 0x07)
payload = StartScan.StartScanEnd(builder)

Packet.PacketStart(builder)
# Packet.PacketAddPayloadType(builder, Payload.Payload().GetMGMTInfo)
Packet.PacketAddPayloadType(builder, Payload.Payload().StartScan)
Packet.PacketAddPayload(builder, payload)
packet = Packet.PacketEnd(builder)

builder.Finish(packet)

buf = builder.Output()
buf = b'\xCA\xFE' + len(buf).to_bytes(2, byteorder='little') + buf
process.stdin.write(buf)

#time.sleep(2)

# builder = flatbuffers.Builder(0)
#
# StopScan.StopScanStart(builder)
# StopScan.StopScanAddControllerId(builder, 0)
# StopScan.StopScanAddAddressType(builder, 0x07)
# payload = StopScan.StopScanEnd(builder)
#
# Packet.PacketStart(builder)
# Packet.PacketAddPayloadType(builder, Payload.Payload().StopScan)
# Packet.PacketAddPayload(builder, payload)
# packet = Packet.PacketEnd(builder)
#
# builder.Finish(packet)
#
# buf2 = builder.Output()
# buf2 = b'\xCA\xFE' + len(buf2).to_bytes(2, byteorder='little') + buf2
# process.stdin.write(buf2)

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

        if packet.PayloadType() == Payload.Payload().GetMGMTInfo:
            getmgmtinfo = GetMGMTInfo.GetMGMTInfo()
            getmgmtinfo.Init(packet.Payload().Bytes, packet.Payload().Pos)
            version = getmgmtinfo.Version()
            revision = getmgmtinfo.Revision()

            print("GetMGMTInfo", "Version:", version, "Revision:", revision)
        elif packet.PayloadType() == Payload.Payload().StartScan:
            startscan = StartScan.StartScan()
            startscan.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = startscan.ControllerId()
            address_type = startscan.AddressType()

            print("StartScan", "Controller id:", controller_id, "Address type:", address_type)
        elif packet.PayloadType() == Payload.Payload().StopScan:
            stopscan = StopScan.StopScan()
            stopscan.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = stopscan.ControllerId()
            address_type = stopscan.AddressType()

            print("StopScan", "Controller id:", controller_id, "Address type:", address_type)
        elif packet.PayloadType() == Payload.Payload().Discovering:
            discovering = Discovering.Discovering()
            discovering.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = discovering.ControllerId()
            address_type = discovering.AddressType()
            state = discovering.State()

            print("Discovering", "Controller id:", controller_id, "Address type:", address_type, "State:", state)

        elif packet.PayloadType() == Payload.Payload().DeviceFound:
            devicefound = DeviceFound.DeviceFound()
            devicefound.Init(packet.Payload().Bytes, packet.Payload().Pos)
            controller_id = devicefound.ControllerId()
            address_type = devicefound.AddressType()
            address = devicefound.Address()
            rssi = devicefound.Rssi()
            flags = devicefound.FlagsAsNumpy()
            uuid = devicefound.Uuid()
            company_id = devicefound.CompanyId()
            device_name = devicefound.DeviceName()

            print("DeviceFound",
                  "Controller id:", controller_id,
                  "Address type:", address_type,
                  "Address:", address,
                  "RSSI:", rssi,
                  "Flags:", flags,
                  "UUID:", uuid,
                  "Company id:", company_id,
                  "Device name:", device_name)
        else:
            print('NOPE...')

        # output = process.stdout.readline()
        # if output == '' and process.poll() is not None:
        #     break
        # if output:
        #     print("STDOUT: {}".format(output))
except KeyboardInterrupt:
    print("Stopping python interface...")

print("Python interface terminated.")
