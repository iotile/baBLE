import trollius as asyncio
from trollius import Future, From

from .BaBLE import Payload
from .flatbuffer import build_packet, extract_packet

# TODO : make changes like in commands_py3.py


@asyncio.coroutine
def start_scan(self, controller_id, on_device_found):

    @asyncio.coroutine
    def on_device_found_event(packet):
        result = extract_packet(
            packet,
            "DeviceFound",
            params=["type", "address", "address_type", "rssi", "uuid", "company_id", "device_name"],
            numpy_params=["manufacturer_data"]
        )

        on_device_found(result)

    @asyncio.coroutine
    def on_start_scan_response(fut, packet):
        print("ON START SCAN RESPONSE", packet.Status())
        del self.callbacks[(Payload.Payload.StartScan, packet.ControllerId())]
        fut.set_result(None)

    fut = Future()

    self.callbacks[(Payload.Payload.DeviceFound, controller_id)] = on_device_found_event
    self.callbacks[(Payload.Payload.StartScan, controller_id)] = (on_start_scan_response, fut)

    # Send command to BaBLEInterface
    self.subprocess.stdin.write(build_packet(
        "StartScan",
        uuid="0001",
        controller_id=controller_id,
        params={"active_scan": True}
    ))

    # wait for discovering = True
    print("Waiting for scan to start...")
    yield From(asyncio.wait_for(fut, 15.0))


@asyncio.coroutine
def stop_scan(self, controller_id):

    @asyncio.coroutine
    def on_stop_scan_response(fut, packet):
        print("ON STOP SCAN RESPONSE", packet.Status())
        del self.callbacks[(Payload.Payload.StopScan, packet.ControllerId())]
        del self.callbacks[(Payload.Payload.DeviceFound, packet.ControllerId())]
        fut.set_result(None)

    fut = Future()

    self.callbacks[(Payload.Payload.StopScan, controller_id)] = (on_stop_scan_response, fut)

    # Send command to BaBLEInterface
    self.subprocess.stdin.write(build_packet("StopScan", uuid="0001", controller_id=controller_id))

    # wait for discovering = True
    print("Waiting for scan to stop...")
    yield From(asyncio.wait_for(fut, 15.0))


@asyncio.coroutine
def probe_services(self, controller_id, connection_handle):

    @asyncio.coroutine
    def on_services_probed(fut, packet):
        print("ON PROBE SERVICES RESPONSE", packet.Status())
        result = extract_packet(packet, "ProbeServices", list_params=["services"])

        services = []
        for service in result["services"]:
            services.append({
                "handle": service.Handle(),
                "group_end_handle": service.GroupEndHandle(),
                "uuid": service.Uuid()
            })

        del self.callbacks[(Payload.Payload.ProbeServices, controller_id)]
        fut.set_result(services)

    fut = Future()

    self.callbacks[(Payload.Payload.ProbeServices, controller_id)] = (on_services_probed, fut)

    # Send command to BaBLEInterface
    self.subprocess.stdin.write(build_packet(
        "ProbeServices",
        uuid="0002",
        controller_id=controller_id,
        params={"connection_handle": connection_handle}
    ))

    # wait for services
    print("Waiting for services...")
    services = yield From(asyncio.wait_for(fut, 15.0))

    return services


@asyncio.coroutine
def probe_characteristics(self, controller_id, connection_handle):

    @asyncio.coroutine
    def on_characteristics_probed(fut, packet):
        print("ON PROBE CHARACTERISTICS RESPONSE", packet.Status())
        result = extract_packet(packet, "ProbeCharacteristics", list_params=["characteristics"])

        characteristics = []
        for characteristic in result["characteristics"]:
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

        del self.callbacks[(Payload.Payload.ProbeCharacteristics, controller_id)]
        fut.set_result(characteristics)

    fut = Future()

    self.callbacks[(Payload.Payload.ProbeCharacteristics, controller_id)] = (on_characteristics_probed, fut)

    # Send command to BaBLEInterface
    self.subprocess.stdin.write(build_packet(
        "ProbeCharacteristics",
        uuid="0003",
        controller_id=controller_id,
        params={"connection_handle": connection_handle}
    ))

    # wait for services
    print("Waiting for characteristics...")
    characteristics = yield From(asyncio.wait_for(fut, 15.0))

    return characteristics


@asyncio.coroutine
def connect(self, controller_id, address, address_type, on_connected_with_info, on_error):

    @asyncio.coroutine
    def on_connected(fut, packet):
        print("ON DEVICE CONNECTED", packet.Status())
        device = extract_packet(packet, "DeviceConnected", ["connection_handle", "address", "address_type"])
        device["controller_id"] = packet.ControllerId()

        fut.set_result(None)
        del self.callbacks[(Payload.Payload.DeviceConnected, device["controller_id"])]

        device["services"] = yield From(probe_services(device["controller_id"], device["connection_handle"]))
        device["characteristics"] = yield From(probe_characteristics(
            device["controller_id"],
            device["connection_handle"]
        ))

        on_connected_with_info(device)

    fut = Future()

    self.callbacks[(Payload.Payload.DeviceConnected, controller_id)] = (on_connected, fut)

    # Send command to BaBLEInterface
    self.subprocess.stdin.write(build_packet(
        "Connect",
        uuid="0001",
        controller_id=controller_id,
        params={"address": address, "address_type": 0 if address_type == "public" else 1}
    ))
    print("Connecting...")

    # TODO: add timeout connection
    try:
        yield From(asyncio.wait_for(fut, 5.0))
    except asyncio.TimeoutError:
        print("CONNECTION TIMEOUT")
        del self.callbacks[(Payload.Payload.DeviceConnected, controller_id)]
        on_error()


@asyncio.coroutine
def disconnect(self, controller_id, connection_handle, on_disconnected):

    @asyncio.coroutine
    def on_device_disconnected(packet):
        print("ON DEVICE DISCONNECTED", packet.Status())
        del self.callbacks[(Payload.Payload.DeviceDisconnected, packet.ControllerId())]

        on_disconnected(connection_handle)

    self.callbacks[(Payload.Payload.DeviceDisconnected, controller_id)] = on_device_disconnected

    # Send command to BaBLEInterface
    self.subprocess.stdin.write(build_packet(
        "Disconnect",
        uuid="0001",
        controller_id=controller_id,
        params={"connection_handle": connection_handle}
    ))

    # wait for discovering = True
    print("Disconnecting...")


@asyncio.coroutine
def cancel_connection(self, controller_id):
    # Registering callback on DeviceFound event
    @asyncio.coroutine
    def on_connection_cancelled(fut, packet):
        print("ON CONNECTION CANCELLED RESPONSE", packet.Status())
        del self.callbacks[(Payload.Payload.CancelConnection, packet.ControllerId())]
        fut.set_result(None)

    fut = Future()

    self.callbacks[(Payload.Payload.CancelConnection, controller_id)] = (on_connection_cancelled, fut)  # TODO: use UUID instead...

    # Send command to BaBLEInterface
    self.subprocess.stdin.write(build_packet("CancelConnection", uuid="0003", controller_id=controller_id))

    # wait for discovering = True
    print("Waiting for connection to cancel...")
    yield From(asyncio.wait_for(fut, 15.0))
