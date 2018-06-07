import threading
import sys
import subprocess
import time

from .BaBLE import Payload, StartScan, DeviceConnected, DeviceFound, StopScan, ProbeServices, \
    ProbeCharacteristics, DeviceDisconnected
from .flatbuffers_packets import fb_start_scan, fb_stop_scan, fb_probe_characteristics, fb_probe_services, \
    fb_disconnect, fb_connect, fb_exit
from .working_thread import WorkingThread
from .receiving_thread import ReceivingThread

PYTHON_2 = sys.version_info[0] < 3

if PYTHON_2:
    import trollius as asyncio
    from trollius import Future
else:
    import asyncio
    from asyncio import Future


@asyncio.coroutine
def start_scan(controller_id, on_device_found):
    # Registering callback on DeviceFound event
    @asyncio.coroutine
    def on_device_found_event(packet):
        response = DeviceFound.DeviceFound()
        response.Init(packet.Payload().Bytes, packet.Payload().Pos)
        result = {
            "type": response.Type(),
            "address_type": response.AddressType(),
            "address": response.Address(),
            "rssi": response.Rssi(),
            "device_uuid": response.Uuid(),
            "company_id": response.CompanyId(),
            "manufacturer_data": response.ManufacturerDataAsNumpy(),
            "device_name": response.DeviceName()
        }
        on_device_found(result)

    @asyncio.coroutine
    def on_start_scan_response(fut, packet):
        print("ON START SCAN RESPONSE", packet.Status())
        response = StartScan.StartScan()
        response.Init(packet.Payload().Bytes, packet.Payload().Pos)
        del BaBLE.callbacks[(Payload.Payload().StartScan, packet.ControllerId())]
        fut.set_result(None)

    fut = Future()

    BaBLE.callbacks[(Payload.Payload().DeviceFound, controller_id)] = on_device_found_event
    BaBLE.callbacks[(Payload.Payload().StartScan, controller_id)] = (on_start_scan_response, fut)

    # Send command to BaBLE
    BaBLE.subprocess.stdin.write(fb_start_scan("0001", controller_id, True))

    # wait for discovering = True
    print("Waiting for scan to start...")
    yield from asyncio.wait_for(fut, 15.0)


@asyncio.coroutine
def stop_scan(controller_id):

    @asyncio.coroutine
    def on_stop_scan_response(fut, packet):
        print("ON STOP SCAN RESPONSE", packet.Status())
        response = StopScan.StopScan()
        response.Init(packet.Payload().Bytes, packet.Payload().Pos)
        del BaBLE.callbacks[(Payload.Payload().StopScan, packet.ControllerId())]
        del BaBLE.callbacks[(Payload.Payload().DeviceFound, packet.ControllerId())]
        fut.set_result(None)

    fut = Future()

    BaBLE.callbacks[(Payload.Payload().StopScan, controller_id)] = (on_stop_scan_response, fut)

    # Send command to BaBLE
    BaBLE.subprocess.stdin.write(fb_stop_scan("0001", controller_id))

    # wait for discovering = True
    print("Waiting for scan to stop...")
    yield from asyncio.wait_for(fut, 15.0)


@asyncio.coroutine
def probe_services(controller_id, connection_handle):

    @asyncio.coroutine
    def on_services_probed(fut, packet):
        print("ON PROBE SERVICES RESPONSE", packet.Status())
        response = ProbeServices.ProbeServices()
        response.Init(packet.Payload().Bytes, packet.Payload().Pos)
        num_services = response.ServicesLength()
        services = []

        for i in range(num_services):
            service = response.Services(i)
            services.append({
                "handle": service.Handle(),
                "group_end_handle": service.GroupEndHandle(),
                "uuid": service.Uuid()
            })

        del BaBLE.callbacks[(Payload.Payload().ProbeServices, controller_id)]
        fut.set_result(services)

    fut = Future()

    BaBLE.callbacks[
        (Payload.Payload().ProbeServices, controller_id)
    ] = (on_services_probed, fut)

    # Send command to BaBLE
    BaBLE.subprocess.stdin.write(fb_probe_services("0002", controller_id, connection_handle))

    # wait for services
    print("Waiting for services...")
    services = yield from asyncio.wait_for(fut, 15.0)

    return services


@asyncio.coroutine
def probe_characteristics(controller_id, connection_handle):

    @asyncio.coroutine
    def on_characteristics_probed(fut, packet):
        print("ON PROBE CHARACTERISTICS RESPONSE", packet.Status())
        response = ProbeCharacteristics.ProbeCharacteristics()
        response.Init(packet.Payload().Bytes, packet.Payload().Pos)
        num_characteristics = response.CharacteristicsLength()
        characteristics = []
        for i in range(num_characteristics):
            characteristic = response.Characteristics(i)
            characteristics.append({
                "handle": characteristic.Handle(),
                "value_handle": characteristic.ValueHandle(),
                "uuid": characteristic.Uuid(),
                "indicate": characteristic.Indicate(),
                "notify": characteristic.Notify(),
                "read": characteristic.Read(),
                "write": characteristic.Write(),
                "broadcast": characteristic.Broadcast()
            })

        del BaBLE.callbacks[(Payload.Payload().ProbeCharacteristics, controller_id)]
        fut.set_result(characteristics)

    fut = Future()

    BaBLE.callbacks[
        (Payload.Payload().ProbeCharacteristics, controller_id)
    ] = (on_characteristics_probed, fut)

    # Send command to BaBLE
    BaBLE.subprocess.stdin.write(fb_probe_characteristics("0003", controller_id, connection_handle))

    # wait for services
    print("Waiting for characteristics...")
    characteristics = yield from asyncio.wait_for(fut, 15.0)

    return characteristics


@asyncio.coroutine
def connect(controller_id, address, address_type, on_connected_with_info, on_error):
    device = {}

    @asyncio.coroutine
    def on_connected(fut, packet):
        print("ON DEVICE CONNECTED", packet.Status())

        response = DeviceConnected.DeviceConnected()
        response.Init(packet.Payload().Bytes, packet.Payload().Pos)

        device["controller_id"] = packet.ControllerId()
        device["connection_handle"] = response.ConnectionHandle()
        device["address"] = response.Address()
        device["address_type"] = response.AddressType()

        fut.set_result(None)
        del BaBLE.callbacks[(Payload.Payload().DeviceConnected, device["controller_id"])]  # TODO: Add address

        device["services"] = yield from probe_services(device["controller_id"], device["connection_handle"])
        device["characteristics"] = yield from probe_characteristics(
            device["controller_id"],
            device["connection_handle"]
        )

        on_connected_with_info(device)

    fut = Future()

    BaBLE.callbacks[(Payload.Payload().DeviceConnected, controller_id)] = (on_connected, fut)

    # Send command to BaBLE
    BaBLE.subprocess.stdin.write(fb_connect("0001", controller_id, address, address_type))
    print("Connecting...")

    # TODO: add timeout connection
    try:
        yield from asyncio.wait_for(fut, 5.0)
    except asyncio.TimeoutError:
        print("CONNECTION TIMEOUT")
        del BaBLE.callbacks[(Payload.Payload().DeviceConnected, controller_id)]
        on_error()


@asyncio.coroutine
def disconnect(controller_id, connection_handle, on_disconnected):

    @asyncio.coroutine
    def on_device_disconnected(packet):
        print("ON DEVICE DISCONNECTED", packet.Status())
        response = DeviceDisconnected.DeviceDisconnected()
        response.Init(packet.Payload().Bytes, packet.Payload().Pos)
        del BaBLE.callbacks[(Payload.Payload().DeviceDisconnected, packet.ControllerId())]

        on_disconnected(connection_handle)

    # TODO: add connection_handle to id callback
    BaBLE.callbacks[(Payload.Payload().DeviceDisconnected, controller_id)] = on_device_disconnected

    # Send command to BaBLE
    BaBLE.subprocess.stdin.write(fb_disconnect("0001", controller_id, connection_handle))

    # wait for discovering = True
    print("Disconnecting...")


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
        BaBLE.subprocess = subprocess.Popen(["../../platforms/linux/build/debug/baBLE_linux", "--logging", "info"],
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
            key = (packet.PayloadType(), packet.ControllerId())
            if key in BaBLE.callbacks:
                val = BaBLE.callbacks[key]
                if isinstance(val, tuple):
                    self.working_thread.add_task(val[0](val[1], packet))
                else:
                    self.working_thread.add_task(val(packet))
            else:
                print("Unexpected packet received:", key)

    def start_scan(self, on_device_found, controller_id=0):
        calldone = threading.Event()

        def scan_started(fut):
            calldone.set()

        self.working_thread.add_task(start_scan(controller_id, on_device_found), scan_started)
        calldone.wait()

    def stop_scan(self, controller_id=0):
        calldone = threading.Event()

        def scan_stopped(fut):
            calldone.set()

        self.working_thread.add_task(stop_scan(controller_id), scan_stopped)
        calldone.wait()

    def connect(self, address, address_type, on_connected, on_error, controller_id=0):
        self.working_thread.add_task(connect(controller_id, address, address_type, on_connected, on_error))

    def disconnect(self, connection_handle, on_disconnected, controller_id=0):
        self.working_thread.add_task(disconnect(controller_id, connection_handle, on_disconnected))
