import asyncio
import struct

from bable_interface.BaBLE import StartScan, StopScan, ProbeServices, ProbeCharacteristics, Connect, Disconnect, \
    CancelConnection, GetConnectedDevices, GetControllersList, Read, Write, WriteWithoutResponse
from bable_interface.BaBLE.StatusCode import StatusCode
from bable_interface.BaBLE.Payload import Payload
from bable_interface.utils import none_cb, to_bytes
from bable_interface.models import BaBLEException, Characteristic, Controller, Packet, PacketUuid, Service


@asyncio.coroutine
def start_scan(self, controller_id, on_device_found, on_scan_started, timeout=15.0):

    @asyncio.coroutine
    def on_device_found_event(packet):
        result = packet.get_dict([
            'controller_id',
            'type',
            'address',
            'address_type',
            'rssi',
            'uuid',
            'company_id',
            'device_name',
            ('manufacturer_data', bytes)
        ])

        on_device_found(True, result, None)

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Start scan response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            on_scan_started(True, packet.get_dict(['controller_id']), None)
            future.set_result(True)
        else:
            self.remove_callback(PacketUuid(payload_type=Payload.DeviceFound, controller_id=controller_id))
            error = BaBLEException(packet, "Failed to start scan")
            on_scan_started(False, None, error)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(StartScan, controller_id=controller_id, active_scan=True)

    self.register_callback(
        PacketUuid(payload_type=Payload.DeviceFound, controller_id=controller_id),
        callback=on_device_found_event
    )
    self.register_callback(
        request_packet.packet_uuid,
        callback=on_response_received,
        params={'future': future}
    )

    self.send_packet(request_packet)

    self.logger.debug("Waiting for scan to start...")
    try:
        result = yield from asyncio.wait_for(future, timeout=timeout)
        return result
    except asyncio.TimeoutError:
        self.remove_callback(
            request_packet.packet_uuid,
            PacketUuid(payload_type=Payload.DeviceFound, controller_id=controller_id)
        )
        on_scan_started(False, None, "Start scan timed out")
        raise RuntimeError("Start scan timed out")


@asyncio.coroutine
def stop_scan(self, controller_id, on_scan_stopped, timeout=15.0):

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Stop scan response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            self.remove_callback(
                PacketUuid(payload_type=Payload.DeviceFound, controller_id=controller_id)
            )
            on_scan_stopped(True, packet.get_dict(['controller_id']), None)
            future.set_result(True)
        else:
            error = BaBLEException(packet, "Failed to stop scan")
            on_scan_stopped(False, None, error)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(StopScan, controller_id=controller_id)

    self.register_callback(
        request_packet.packet_uuid,
        callback=on_response_received,
        params={'future': future}
    )

    self.send_packet(request_packet)

    self.logger.debug("Waiting for scan to stop...")
    try:
        result = yield from asyncio.wait_for(future, timeout=timeout)
        return result
    except asyncio.TimeoutError:
        self.remove_callback(request_packet.packet_uuid)
        on_scan_stopped(False, None, "Stop scan timed out")
        raise RuntimeError("Stop scan timed out")


@asyncio.coroutine
def probe_services(self, controller_id, connection_handle, timeout=15.0):

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Probe services response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            services = packet.get(
                name='services',
                format_function=lambda raw_services: [Service(raw_service) for raw_service in raw_services]
            )

            future.set_result(services)
        else:
            future.set_exception(BaBLEException(packet, "Failed to probe services"))

    future = asyncio.Future()
    request_packet = Packet.build(ProbeServices, controller_id=controller_id, connection_handle=connection_handle)

    self.register_callback(request_packet.packet_uuid, callback=on_response_received, params={'future': future})

    self.send_packet(request_packet)

    self.logger.debug("Waiting for services...")
    try:
        services = yield from asyncio.wait_for(future, timeout=timeout)
        return services
    except asyncio.TimeoutError:
        self.remove_callback(request_packet.packet_uuid)
        raise RuntimeError("Probe services timed out")


@asyncio.coroutine
def probe_characteristics(self, controller_id, connection_handle, timeout=15.0):

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Probe characteristics response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            characteristics = packet.get(
                name='characteristics',
                format_function=lambda raw_chars: [Characteristic(raw_char) for raw_char in raw_chars]
            )

            future.set_result(characteristics)
        else:
            future.set_exception(BaBLEException(packet, "Failed to probe characteristics"))

    future = asyncio.Future()
    request_packet = Packet.build(
        ProbeCharacteristics,
        controller_id=controller_id,
        connection_handle=connection_handle
    )

    self.register_callback(request_packet.packet_uuid, callback=on_response_received, params={'future': future})

    self.send_packet(request_packet)

    self.logger.debug("Waiting for characteristics...")
    try:
        characteristics = yield from asyncio.wait_for(future, timeout=timeout)
        return characteristics
    except asyncio.TimeoutError:
        self.remove_callback(request_packet.packet_uuid)
        raise RuntimeError("Probe characteristics timed out")


@asyncio.coroutine
def connect(self, controller_id, address, address_type, on_connected_with_info, on_disconnected, timeout=15.0):

    connected_event_uuid = PacketUuid(
        payload_type=Payload.DeviceConnected,
        controller_id=controller_id,
        address=address
    )

    @asyncio.coroutine
    def on_unexpected_disconnection(packet):
        self.logger.info("Unexpected disconnection event received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        data = packet.get_dict([
            'controller_id',
            'connection_handle',
            'reason'
        ])
        on_disconnected(True, data, None)

    @asyncio.coroutine
    def on_connected(packet, future):
        self.logger.debug("Device connected event received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            device = packet.get_dict([
                'controller_id',
                'connection_handle',
                'address',
                'address_type'
            ])

            self.register_callback(
                PacketUuid(payload_type=Payload.DeviceDisconnected,
                           controller_id=controller_id,
                           connection_handle=device['connection_handle']),
                callback=on_unexpected_disconnection
            )

            device['services'] = yield from self.probe_services(controller_id, device['connection_handle'])
            device['characteristics'] = yield from self.probe_characteristics(
                controller_id,
                device['connection_handle']
            )

            on_connected_with_info(True, device, None)

            future.set_result(device)
        else:
            error = BaBLEException(packet, "Failed to connect", address=address)
            on_connected_with_info(False, None, error)
            future.set_exception(error)

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Connect response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code != StatusCode.Success:
            self.remove_callback(connected_event_uuid)
            error = BaBLEException(packet, "Failed to connect", address=address)
            on_connected_with_info(False, None, error)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(
        Connect,
        controller_id=controller_id,
        address=address,
        address_type=0 if address_type == 'public' else 1
    )

    self.register_callback(request_packet.packet_uuid, callback=on_response_received, params={'future': future})
    self.register_callback(connected_event_uuid, callback=on_connected, params={'future': future})

    self.send_packet(request_packet)

    self.logger.debug("Connecting...")
    try:
        result = yield from asyncio.wait_for(future, timeout=timeout)
        return result
    except asyncio.TimeoutError:
        self.remove_callback(connected_event_uuid)
        on_connected_with_info(False, None, "Connection timed out")
        raise RuntimeError("Connection timed out")


@asyncio.coroutine
def disconnect(self, controller_id, connection_handle, on_disconnected, timeout=15.0):

    disconnected_event_uuid = PacketUuid(
        payload_type=Payload.DeviceDisconnected,
        controller_id=controller_id,
        connection_handle=connection_handle
    )

    @asyncio.coroutine
    def on_device_disconnected(packet, future):
        self.logger.debug("Device disconnected event received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            data = packet.get_dict([
                'controller_id',
                'connection_handle',
                'reason'
            ])

            on_disconnected(True, data, None)

            future.set_result(data)
        else:
            error = BaBLEException(packet, "Failed to disconnect")
            on_disconnected(False, None, error)
            future.set_exception(error)

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Disconnect response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code != StatusCode.Success:
            self.remove_callback(disconnected_event_uuid)

            error = BaBLEException(packet, "Failed to disconnect")
            on_disconnected(False, None, error)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(Disconnect, controller_id=controller_id, connection_handle=connection_handle)

    self.register_callback(request_packet.packet_uuid, callback=on_response_received, params={'future': future})
    self.register_callback(
        disconnected_event_uuid,
        callback=on_device_disconnected,
        params={'future': future},
        replace=True
    )

    self.send_packet(request_packet)

    self.logger.debug("Disconnecting...")
    try:
        result = yield from asyncio.wait_for(future, timeout=timeout)
        return result
    except asyncio.TimeoutError:
        self.remove_callback(disconnected_event_uuid)
        on_disconnected(False, None, "Disconnection timed out")
        raise RuntimeError("Disconnection timed out")


@asyncio.coroutine
def cancel_connection(self, controller_id, on_connection_cancelled, timeout=15.0):

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Cancel connection response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            on_connection_cancelled(True, None, None)
            future.set_result(True)
        else:
            error = BaBLEException(packet, "Failed to cancel connection")
            on_connection_cancelled(False, None, error)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(CancelConnection, controller_id=controller_id)

    self.register_callback(request_packet.packet_uuid, callback=on_response_received, params={'future': future})

    self.send_packet(request_packet)

    self.logger.debug("Waiting for connection to cancel...")
    try:
        result = yield from asyncio.wait_for(future, timeout=timeout)
        return result
    except asyncio.TimeoutError:
        self.remove_callback(request_packet.packet_uuid)
        on_connection_cancelled(False, None, "Cancel connection timed out")
        raise RuntimeError("Cancel connection timed out")


@asyncio.coroutine
def list_connected_devices(self, controller_id, timeout=15.0):

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("List of connected devices received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            devices = packet.get('devices')

            future.set_result(devices)
        else:
            future.set_exception(BaBLEException(packet, "Failed to list connected devices"))

    future = asyncio.Future()
    request_packet = Packet.build(GetConnectedDevices, controller_id=controller_id)

    self.register_callback(request_packet.packet_uuid, callback=on_response_received, params={'future': future})

    self.send_packet(request_packet)

    self.logger.debug("Waiting for list of connected devices...")
    try:
        connected_devices = yield from asyncio.wait_for(future, timeout=timeout)
        return connected_devices
    except asyncio.TimeoutError:
        self.remove_callback(request_packet.packet_uuid)
        raise RuntimeError("List connected devices timed out")


@asyncio.coroutine
def list_controllers(self, timeout=15.0):

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("List of controllers received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            controllers = packet.get(
                name='controllers',
                format_function=lambda raw_ctrls: [Controller(raw_ctrl) for raw_ctrl in raw_ctrls]
            )

            future.set_result(controllers)
        else:
            future.set_exception(BaBLEException(packet, "Failed to list controllers"))

    future = asyncio.Future()
    request_packet = Packet.build(GetControllersList)

    self.register_callback(
        request_packet.packet_uuid,
        callback=on_response_received,
        params={'future': future}
    )

    self.send_packet(request_packet)

    self.logger.debug("Waiting for list of controllers...")
    try:
        controllers = yield from asyncio.wait_for(future, timeout=timeout)
        return controllers
    except asyncio.TimeoutError:
        self.remove_callback(request_packet.packet_uuid)
        raise RuntimeError("List controllers timed out")


@asyncio.coroutine
def read(self, controller_id, connection_handle, attribute_handle, on_read, timeout=15.0):

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Read response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            data = packet.get_dict([
                'controller_id',
                'connection_handle',
                'attribute_handle',
                ('value', bytes)
            ])

            on_read(True, data, None)

            future.set_result(data)
        else:
            error = BaBLEException(packet, "Failed to read value",
                                   connection_handle=connection_handle,
                                   attribute_handle=attribute_handle)
            on_read(False, None, error)

            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(
        Read,
        controller_id=controller_id,
        connection_handle=connection_handle,
        attribute_handle=attribute_handle
    )

    self.register_callback(request_packet.packet_uuid, callback=on_response_received, params={'future': future})

    self.send_packet(request_packet)

    self.logger.debug("Reading...")
    try:
        result = yield from asyncio.wait_for(future, timeout=timeout)
        return result
    except asyncio.TimeoutError:
        self.remove_callback(request_packet.packet_uuid)
        on_read(False, None, "Read timed out")
        raise RuntimeError("Read timed out")


@asyncio.coroutine
def write(self, controller_id, connection_handle, attribute_handle, value, on_written, timeout=15.0):

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Write response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            data = packet.get_dict([
                'controller_id',
                'connection_handle',
                'attribute_handle'
            ])

            on_written(True, data, None)

            future.set_result(data)
        else:
            error = BaBLEException(packet, "Failed to write value",
                                   connection_handle=connection_handle,
                                   attribute_handle=attribute_handle)
            on_written(False, None, error)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(
        Write,
        controller_id=controller_id,
        connection_handle=connection_handle,
        attribute_handle=attribute_handle,
        value=value
    )

    self.register_callback(request_packet.packet_uuid, callback=on_response_received, params={'future': future})

    self.send_packet(request_packet)

    self.logger.debug("Writing...")
    try:
        result = yield from asyncio.wait_for(future, timeout=timeout)
        return result
    except asyncio.TimeoutError:
        self.remove_callback(request_packet.packet_uuid)
        on_written(False, None, "Write timed out")
        raise RuntimeError("Write timed out")


@asyncio.coroutine
def write_without_response(self, controller_id, connection_handle, attribute_handle, value):
    request_packet = Packet.build(
        WriteWithoutResponse,
        controller_id=controller_id,
        connection_handle=connection_handle,
        attribute_handle=attribute_handle,
        value=value
    )

    self.send_packet(request_packet)

    self.logger.debug("Write without response command sent")


@asyncio.coroutine
def set_notification(self, state, controller_id, connection_handle, attribute_handle, on_notification_received,
                     timeout=15.0):

    notification_event_uuid = PacketUuid(payload_type=Payload.NotificationReceived,
                                         controller_id=controller_id,
                                         connection_handle=connection_handle,
                                         attribute_handle=attribute_handle)

    @asyncio.coroutine
    def on_notification_event(packet):
        result = packet.get_dict([
            'connection_handle',
            'attribute_handle',
            ('value', bytes)
        ])

        on_notification_received(True, result, None)

    try:
        read_result = yield from self.read(controller_id, connection_handle, attribute_handle, none_cb, timeout)
    except Exception as e:
        on_notification_received(False, None, "Error while reading notification configuration (exception={})".format(e))
        raise RuntimeError("Error while reading notification configuration (exception={})".format(e))

    current_state = struct.unpack('H', read_result['value'])[0]

    if state:
        self.register_callback(notification_event_uuid, callback=on_notification_event)
        new_state = current_state | 1
    else:
        self.remove_callback(notification_event_uuid)
        new_state = current_state & 0xFFFE

    value = to_bytes(new_state, 2, byteorder='little')

    try:
        result = yield from self.write(controller_id, connection_handle, attribute_handle, value, none_cb, timeout)
        return result
    except Exception as e:
        if state:
            self.remove_callback(notification_event_uuid)
        on_notification_received(False, None, "Error while writing notification configuration (exception={})".format(e))
        raise RuntimeError("Error while writing notification configuration (exception={})".format(e))


@asyncio.coroutine
def handle_error(self, packet, on_error):
    on_error(packet.full_status, packet.get('message'))
