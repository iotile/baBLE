import flatbuffers
import functools
import threading
import sys
import subprocess
import struct
import time

from .BaBLE import Packet, Payload, Discovering, StartScan, DeviceFound, Exit, StopScan

PYTHON_2 = sys.version_info[0] < 3

if PYTHON_2:
    import trollius as asyncio
    from trollius import Future

    from .commands_py2 import connect

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
    import asyncio
    from concurrent.futures import Future
    from .commands_py3 import connect

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


## Exit
def fb_exit():
    builder = flatbuffers.Builder(0)
    Exit.ExitStart(builder)
    payload = Exit.ExitEnd(builder)

    return build_packet(builder, "", payload, Payload.Payload.Exit)


def fb_start_scan(uuid, controller_id):
    builder = flatbuffers.Builder(0)

    StartScan.StartScanStart(builder)
    StartScan.StartScanAddAddressType(builder, 0x07)
    payload = StartScan.StartScanEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.StartScan, controller_id)


def fb_stop_scan(uuid, controller_id):
    builder = flatbuffers.Builder(0)
    StopScan.StopScanStart(builder)
    StopScan.StopScanAddAddressType(builder, 0x07)
    payload = StopScan.StopScanEnd(builder)

    return build_packet(builder, uuid, payload, Payload.Payload.StopScan, controller_id)


@asyncio.coroutine
def start_scan(on_discovered):
    # Registering callback on DeviceFound event
    def on_device_found_event(packet):
        devicefound = DeviceFound.DeviceFound()
        devicefound.Init(packet.Payload().Bytes, packet.Payload().Pos)
        result = {
            "address_type": devicefound.AddressType(),
            "address": devicefound.Address(),
            "rssi": devicefound.Rssi(),
            "flags": devicefound.FlagsAsNumpy(),
            "device_uuid": devicefound.Uuid(),
            "company_id": devicefound.CompanyId(),
            "manufacturer_data_advertised": devicefound.ManufacturerDataAdvertisedAsNumpy(),
            "manufacturer_data_scanned": devicefound.ManufacturerDataScannedAsNumpy(),
            "device_name": devicefound.DeviceName()
        }
        on_discovered(result)

    waiting_scan_started = threading.Event()

    def on_discovering(packet):
        discovering = Discovering.Discovering()
        discovering.Init(packet.Payload().Bytes, packet.Payload().Pos)
        state = discovering.State()
        print("ON DISCOVERING", state)
        if state is True:
            waiting_scan_started.set()
            del BaBLE.callbacks[Payload.Payload().Discovering]

    BaBLE.callbacks[Payload.Payload().DeviceFound] = on_device_found_event
    BaBLE.callbacks[Payload.Payload().Discovering] = on_discovering

    # Send command to BaBLE
    BaBLE.subprocess.stdin.write(fb_start_scan("0001", 0))

    # wait for discovering = True
    print("Waiting for scan to start...")
    waiting_scan_started.wait()


@asyncio.coroutine
def stop_scan():
    waiting_scan_stopped = threading.Event()

    def on_discovering(packet):
        discovering = Discovering.Discovering()
        discovering.Init(packet.Payload().Bytes, packet.Payload().Pos)
        state = discovering.State()
        print("ON DISCOVERING", state)
        if state is False:
            waiting_scan_stopped.set()
            del BaBLE.callbacks[Payload.Payload().Discovering]

    BaBLE.callbacks[Payload.Payload().Discovering] = on_discovering

    # Send command to BaBLE
    BaBLE.subprocess.stdin.write(fb_stop_scan("0001", 0))

    # wait for discovering = True
    print("Waiting for scan to stop...")
    waiting_scan_stopped.wait()


class BaBLE(object):

    subprocess = None
    callbacks = {}

    def __init__(self):
        self.working_ready_event = threading.Event()
        self.working_thread = WorkingThread(self.working_ready_event)

        self.receiving_thread = None
        self.stop_receiving_event = threading.Event()

        self.subprocess_ready_event = threading.Event()

    def start(self):
        self.working_thread.start()
        self.working_ready_event.wait()
        BaBLE.subprocess = subprocess.Popen(["../../platforms/linux/build/baBLE_linux"],
                                            stdout=subprocess.PIPE, stdin=subprocess.PIPE,
                                            bufsize=0,
                                            universal_newlines=False)

        self.receiving_thread = ReceivingThread(
            self.stop_receiving_event,
            self.on_receive,
            BaBLE.subprocess.stdout
        )
        self.receiving_thread.setDaemon(True)
        self.receiving_thread.start()

        self.subprocess_ready_event.wait()

    def stop(self):
        self.working_thread.stop()
        BaBLE.subprocess.stdin.write(fb_exit())
        while BaBLE.subprocess.poll() is None:
            print("Waiting for subprocess to stop")
            time.sleep(0.1)
        self.receiving_thread.join(5)
        if self.receiving_thread.isAlive():
            print("Timeout while waiting for receiving thread to stop...")

    def on_receive(self, packet):
        if packet.PayloadType() == Payload.Payload().Ready:
            self.subprocess_ready_event.set()
        else:
            if packet.PayloadType() in BaBLE.callbacks:
                BaBLE.callbacks[packet.PayloadType()](packet)
            else:
                print("Unexpected packet received:", packet.PayloadType())

    def start_scan(self, on_device_found):
        calldone = threading.Event()

        def scan_started(fut):
            calldone.set()

        self.working_thread.add_task(start_scan(on_device_found), scan_started)
        calldone.wait()

    def stop_scan(self):
        calldone = threading.Event()

        def scan_stopped(fut):
            calldone.set()

        self.working_thread.add_task(stop_scan(), scan_stopped)
        calldone.wait()

    def connect(self, address):
        self.working_thread.add_task(connect(address))


class WorkingThread(threading.Thread):

    def __init__(self, ready_event):
        super(WorkingThread, self).__init__()
        self.loop = asyncio.get_event_loop()
        self.thread_id = None
        self.ready_event = ready_event

    def run(self):
        self.loop = asyncio.new_event_loop()
        asyncio.set_event_loop(self.loop)
        self.thread_id = threading.current_thread()
        self.loop.call_soon(self.ready_event.set)
        self.loop.run_forever()

    def stop(self):
        self.loop.call_soon_threadsafe(self.loop.stop)

    def add_task(self, task, callback=None):
        def _async_add(func):
            try:
                result = func()
                if callback is not None:
                    result.add_done_callback(callback)
            except Exception as e:
                print(e)

        func = functools.partial(asyncio.ensure_future, task, loop=self.loop)
        if threading.current_thread() == self.thread_id:
            result = func()  # We can call directly if we're not going between threads.
            if callback is not None:
                result.add_done_callback(callback)
        else:
            self.loop.call_soon_threadsafe(_async_add, func)

    def cancel_task(self, task):
        self.loop.call_soon_threadsafe(task.cancel)


class ReceivingThread(threading.Thread):

    def __init__(self, stop_event, on_receive, file):
        super(ReceivingThread, self).__init__()
        self.stop_event = stop_event
        self.on_receive = on_receive
        self.file = file

    def run(self):
        try:
            while True:
                header = bytearray()
                while len(header) < 4:
                    header += self.file.read(1)

                if header[:2] != b'\xCA\xFE':
                    print('ERROR')
                    print(header[:2])
                    continue

                length = (header[3] << 8) | header[2]  # depends on ENDIANNESS

                # Needs timeout
                payload = bytearray()
                while len(payload) < length:
                    payload += self.file.read(1)

                packet = Packet.Packet.GetRootAsPacket(payload, 0)
                if packet.PayloadType() == Payload.Payload().Exit:
                    break

                self.on_receive(packet)
        except Exception as e:
            print(e)
