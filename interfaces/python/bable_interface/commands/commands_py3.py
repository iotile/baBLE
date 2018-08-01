import asyncio
import struct
from uuid import UUID
from bable_interface.BaBLE import StartScan, StopScan, ProbeServices, ProbeCharacteristics, Connect, Disconnect, \
    CancelConnection, GetConnectedDevices, GetControllersList, ReadCentral, SetAdvertising, SetGATTTable, \
    WriteCentral, WriteWithoutResponseCentral, EmitNotification
from bable_interface.BaBLE.StatusCode import StatusCode
from bable_interface.BaBLE.Payload import Payload
from bable_interface.utils import none_cb, string_to_uuid, string_types, switch_endianness_string, uuid_to_string,\
    to_bytes
from bable_interface.models import BaBLEException, Characteristic, Controller, Device, Packet, PacketUuid, Service


@asyncio.coroutine
def start_scan(self, controller_id, active_scan, on_device_found, on_scan_started, timeout):

    if isinstance(on_device_found, (tuple, list)):
        on_device_found_cb = on_device_found[0]
        on_device_found_params = on_device_found[1:]
    else:
        on_device_found_cb = on_device_found
        on_device_found_params = []

    if isinstance(on_scan_started, (tuple, list)):
        on_scan_started_cb = on_scan_started[0]
        on_scan_started_params = on_scan_started[1:]
    else:
        on_scan_started_cb = on_scan_started
        on_scan_started_params = []

    @asyncio.coroutine
    def on_device_found_event(packet):
        result = packet.get_dict([
            'controller_id',
            'type',
            ('address', lambda value: value.decode()),
            ('address_type', lambda value: 'public' if value == 0 else 'random'),
            'rssi',
            ('uuid', lambda value: string_to_uuid(value, input_byteorder='little')),
            'company_id',
            ('device_name', lambda value: value.decode()),
            ('manufacturer_data', bytes)
        ])

        on_device_found_cb(True, result, None, *on_device_found_params)

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Start scan response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            on_scan_started_cb(True, packet.get_dict(['controller_id']), None, *on_scan_started_params)
            future.set_result(True)
        else:
            self.remove_callback(PacketUuid(payload_type=Payload.DeviceFound, controller_id=controller_id))
            error = BaBLEException(packet, "Failed to start scan")
            on_scan_started_cb(False, None, error, *on_scan_started_params)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(StartScan, controller_id=controller_id, active_scan=active_scan)

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
        self.remove_callback([
            request_packet.packet_uuid,
            PacketUuid(payload_type=Payload.DeviceFound, controller_id=controller_id)
        ])
        on_scan_started_cb(False, None, "Start scan timed out", *on_scan_started_params)
        raise RuntimeError("Start scan timed out")


@asyncio.coroutine
def stop_scan(self, controller_id, on_scan_stopped, timeout):

    if isinstance(on_scan_stopped, (tuple, list)):
        on_scan_stopped_cb = on_scan_stopped[0]
        on_scan_stopped_params = on_scan_stopped[1:]
    else:
        on_scan_stopped_cb = on_scan_stopped
        on_scan_stopped_params = []

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Stop scan response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            self.remove_callback(PacketUuid(payload_type=Payload.DeviceFound, controller_id=controller_id))
            on_scan_stopped_cb(True, packet.get_dict(['controller_id']), None, *on_scan_stopped_params)
            future.set_result(True)
        else:
            error = BaBLEException(packet, "Failed to stop scan")
            on_scan_stopped_cb(False, None, error, *on_scan_stopped_params)
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
        on_scan_stopped_cb(False, None, "Stop scan timed out", *on_scan_stopped_params)
        raise RuntimeError("Stop scan timed out")


@asyncio.coroutine
def probe_services(self, controller_id, connection_handle, on_services_probed, timeout):

    if isinstance(on_services_probed, (tuple, list)):
        on_services_probed_cb = on_services_probed[0]
        on_services_probed_params = on_services_probed[1:]
    else:
        on_services_probed_cb = on_services_probed
        on_services_probed_params = []

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Probe services response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            services = packet.get_dict([
                'controller_id',
                'connection_handle',
                ('services', lambda services: [Service.from_flatbuffers(service) for service in services])
            ])
            on_services_probed_cb(True, services, None, *on_services_probed_params)
            future.set_result(services)
        else:
            error = BaBLEException(packet, "Failed to probe services", connection_handle=connection_handle)
            on_services_probed_cb(False, None, error, *on_services_probed_params)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(
        ProbeServices,
        controller_id=controller_id,
        connection_handle=connection_handle
    )

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
def probe_characteristics(self, controller_id, connection_handle, start_handle, end_handle, on_characteristics_probed,
                          timeout):

    if isinstance(on_characteristics_probed, (tuple, list)):
        on_characteristics_probed_cb = on_characteristics_probed[0]
        on_characteristics_probed_params = on_characteristics_probed[1:]
    else:
        on_characteristics_probed_cb = on_characteristics_probed
        on_characteristics_probed_params = []

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Probe characteristics response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            characteristics = packet.get_dict([
                'controller_id',
                'connection_handle',
                ('characteristics', lambda chars: [Characteristic.from_flatbuffers(char) for char in chars])
            ])
            on_characteristics_probed_cb(True, characteristics, None, *on_characteristics_probed_params)
            future.set_result(characteristics)
        else:
            error = BaBLEException(packet, "Failed to probe characteristics", connection_handle=connection_handle)
            on_characteristics_probed_cb(False, None, error, *on_characteristics_probed_params)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(
        ProbeCharacteristics,
        controller_id=controller_id,
        connection_handle=connection_handle,
        start_handle=start_handle,
        end_handle=end_handle
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
def connect(self, controller_id, address, address_type, connection_interval, on_connected_with_info, on_disconnected,
            timeout):

    if not isinstance(connection_interval, (tuple, list)) \
       or len(connection_interval) != 2 \
       or not all(isinstance(v, (int, float)) for v in connection_interval):
        raise ValueError("connection_interval must be a 2-number tuple or list ([min, max])")

    connected_event_uuid = PacketUuid(
        payload_type=Payload.DeviceConnected,
        controller_id=controller_id,
        address=address
    )

    if isinstance(on_connected_with_info, (tuple, list)):
        on_connected_cb = on_connected_with_info[0]
        on_connected_params = on_connected_with_info[1:]
    else:
        on_connected_cb = on_connected_with_info
        on_connected_params = []

    if isinstance(on_disconnected, (tuple, list)):
        on_disconnected_cb = on_disconnected[0]
        on_disconnected_params = on_disconnected[1:]
    else:
        on_disconnected_cb = on_disconnected
        on_disconnected_params = []

    @asyncio.coroutine
    def on_unexpected_disconnection(packet):
        self.logger.info("Unexpected disconnection event received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        data = packet.get_dict([
            'controller_id',
            'connection_handle',
            'reason',
            'code'
        ])
        self.remove_callback(
            PacketUuid(controller_id=controller_id, connection_handle=data['connection_handle']),
            match_connection_only=True
        )
        on_disconnected_cb(True, data, None, *on_disconnected_params)

    @asyncio.coroutine
    def on_connected(packet, future):
        self.logger.debug("Device connected event received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            device = packet.get_dict([
                'controller_id',
                'connection_handle',
                'address',
                ('address_type', lambda value: 'public' if value == 0 else 'random')
            ])

            self.register_callback(
                PacketUuid(payload_type=Payload.DeviceDisconnected,
                           controller_id=controller_id,
                           connection_handle=device['connection_handle']),
                callback=on_unexpected_disconnection
            )

            on_connected_cb(True, device, None, *on_connected_params)
            future.set_result(device)
        else:
            error = BaBLEException(packet, "Failed to connect", address=address)
            on_connected_cb(False, None, error, *on_connected_params)
            future.set_exception(error)
            return

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Connect response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code != StatusCode.Success:
            self.remove_callback(connected_event_uuid)
            error = BaBLEException(packet, "Failed to connect", address=address)
            on_connected_cb(False, None, error, *on_connected_params)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(
        Connect,
        controller_id=controller_id,
        address=address,
        address_type=0 if address_type == 'public' else 1,
        connection_interval_min=connection_interval[0],
        connection_interval_max=connection_interval[1]
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
        on_connected_cb(False, None, "Connection timed out", *on_connected_params)
        raise RuntimeError("Connection timed out")


@asyncio.coroutine
def disconnect(self, controller_id, connection_handle, on_disconnected, timeout):

    disconnected_event_uuid = PacketUuid(
        payload_type=Payload.DeviceDisconnected,
        controller_id=controller_id,
        connection_handle=connection_handle
    )

    if isinstance(on_disconnected, (tuple, list)):
        on_disconnected_cb = on_disconnected[0]
        on_disconnected_params = on_disconnected[1:]
    else:
        on_disconnected_cb = on_disconnected
        on_disconnected_params = []

    @asyncio.coroutine
    def on_device_disconnected(packet, future):
        self.logger.debug("Device disconnected event received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            data = packet.get_dict([
                'controller_id',
                'connection_handle',
                'reason',
                'code'
            ])

            self.remove_callback(
                PacketUuid(controller_id=controller_id, connection_handle=connection_handle),
                match_connection_only=True
            )
            on_disconnected_cb(True, data, None, *on_disconnected_params)
            future.set_result(data)
        else:
            error = BaBLEException(packet, "Failed to disconnect")
            on_disconnected_cb(False, None, error, *on_disconnected_params)
            future.set_exception(error)

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Disconnect response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code != StatusCode.Success:
            self.remove_callback(disconnected_event_uuid)

            error = BaBLEException(packet, "Failed to disconnect")
            on_disconnected_cb(False, None, error, *on_disconnected_params)
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
        on_disconnected_cb(False, None, "Disconnection timed out", *on_disconnected_params)
        raise RuntimeError("Disconnection timed out")


@asyncio.coroutine
def cancel_connection(self, controller_id, on_connection_cancelled, timeout):

    if isinstance(on_connection_cancelled, (tuple, list)):
        on_connection_cancelled_cb = on_connection_cancelled[0]
        on_connection_cancelled_params = on_connection_cancelled[1:]
    else:
        on_connection_cancelled_cb = on_connection_cancelled
        on_connection_cancelled_params = []

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Cancel connection response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            on_connection_cancelled_cb(True, None, None, *on_connection_cancelled_params)
            future.set_result(True)
        else:
            error = BaBLEException(packet, "Failed to cancel connection")
            on_connection_cancelled_cb(False, None, error, *on_connection_cancelled_params)
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
        on_connection_cancelled_cb(False, None, "Cancel connection timed out", *on_connection_cancelled_params)
        raise RuntimeError("Cancel connection timed out")


@asyncio.coroutine
def list_connected_devices(self, controller_id, timeout):

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("List of connected devices received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            devices = packet.get(
                name='devices',
                format_function=lambda raw_devs: [Device.from_flatbuffers(raw_dev) for raw_dev in raw_devs]
            )

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
def list_controllers(self, timeout):

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("List of controllers received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            controllers = packet.get(
                name='controllers',
                format_function=lambda raw_ctrls: [Controller.from_flatbuffers(raw_ctrl) for raw_ctrl in raw_ctrls]
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
def read(self, controller_id, connection_handle, attribute_handle, on_read, timeout):

    if isinstance(on_read, (tuple, list)):
        on_read_cb = on_read[0]
        on_read_params = on_read[1:]
    else:
        on_read_cb = on_read
        on_read_params = []

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

            on_read_cb(True, data, None, *on_read_params)

            future.set_result(data)
        else:
            error = BaBLEException(packet, "Failed to read value",
                                   connection_handle=connection_handle,
                                   attribute_handle=attribute_handle)
            on_read_cb(False, None, error, *on_read_params)

            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(
        ReadCentral,
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
        on_read_cb(False, None, "Read timed out", *on_read_params)
        raise RuntimeError("Read timed out")


@asyncio.coroutine
def write(self, controller_id, connection_handle, attribute_handle, value, on_written, timeout):

    if isinstance(on_written, (tuple, list)):
        on_written_cb = on_written[0]
        on_written_params = on_written[1:]
    else:
        on_written_cb = on_written
        on_written_params = []

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

            on_written_cb(True, data, None, *on_written_params)

            future.set_result(data)
        else:
            error = BaBLEException(packet, "Failed to write value",
                                   connection_handle=connection_handle,
                                   attribute_handle=attribute_handle)
            on_written_cb(False, None, error, *on_written_params)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(
        WriteCentral,
        controller_id=controller_id,
        connection_handle=connection_handle,
        attribute_handle=attribute_handle,
        value=bytes(value)
    )

    self.register_callback(request_packet.packet_uuid, callback=on_response_received, params={'future': future})

    self.send_packet(request_packet)

    self.logger.debug("Writing...")
    try:
        result = yield from asyncio.wait_for(future, timeout=timeout)
        return result
    except asyncio.TimeoutError:
        self.remove_callback(request_packet.packet_uuid)
        on_written_cb(False, None, "Write timed out", *on_written_params)
        raise RuntimeError("Write timed out")


@asyncio.coroutine
def write_without_response(self, controller_id, connection_handle, attribute_handle, value, timeout):

    @asyncio.coroutine
    def on_ack_received(packet, future):
        self.logger.debug("WriteWithoutResponse ack received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            future.set_result(True)
        else:
            error = BaBLEException(packet, "Failed to send WriteWithoutResponse packet",
                                   connection_handle=connection_handle,
                                   attribute_handle=attribute_handle)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(
        WriteWithoutResponseCentral,
        controller_id=controller_id,
        connection_handle=connection_handle,
        attribute_handle=attribute_handle,
        value=bytes(value)
    )

    self.register_callback(request_packet.packet_uuid, callback=on_ack_received, params={'future': future})

    self.send_packet(request_packet)

    try:
        result = yield from asyncio.wait_for(future, timeout=timeout)
        self.logger.debug("Write without response command sent")
        return result
    except asyncio.TimeoutError:
        self.remove_callback(request_packet.packet_uuid)
        raise RuntimeError("WriteWithoutResponse timed out")


@asyncio.coroutine
def set_notification(self, controller_id, enabled, connection_handle, characteristic, on_notification_set,
                     on_notification_received, timeout):

    if not isinstance(characteristic, Characteristic):
        raise ValueError("Characteristic parameter must be a 'bable_interface.models.Characteristic' object")

    notification_event_uuid = PacketUuid(payload_type=Payload.NotificationReceived,
                                         controller_id=controller_id,
                                         connection_handle=connection_handle,
                                         attribute_handle=characteristic.value_handle)

    if isinstance(on_notification_set, (tuple, list)):
        on_notification_set_cb = on_notification_set[0]
        on_notification_set_params = on_notification_set[1:]
    else:
        on_notification_set_cb = on_notification_set
        on_notification_set_params = []

    if isinstance(on_notification_received, (tuple, list)):
        on_notification_received_cb = on_notification_received[0]
        on_notification_received_params = on_notification_received[1:]
    else:
        on_notification_received_cb = on_notification_received
        on_notification_received_params = []

    @asyncio.coroutine
    def on_notification_event(packet):
        result = packet.get_dict([
            'controller_id',
            'connection_handle',
            'attribute_handle',
            ('value', bytes)
        ])

        on_notification_received_cb(True, result, None, *on_notification_received_params)

    try:
        read_result = yield from self.read(controller_id, connection_handle, characteristic.config_handle, none_cb,
                                           timeout)
    except (RuntimeError, BaBLEException) as err:
        on_notification_set_cb(
            False,
            None,
            "Error while reading notification configuration (exception={})".format(err),
            *on_notification_set_params
        )
        raise RuntimeError("Error while reading notification config (exception={})".format(err))

    current_state = struct.unpack('H', read_result['value'])[0]

    if enabled:
        self.register_callback(notification_event_uuid, callback=on_notification_event)
        new_state = current_state | 1
    else:
        self.remove_callback(notification_event_uuid)
        new_state = current_state & 0xFFFE

    if new_state == current_state:
        on_notification_set_cb(True, read_result, None, *on_notification_set_params)
        return read_result

    value = to_bytes(new_state, 2, byteorder='little')

    try:
        result = yield from self.write(controller_id, connection_handle, characteristic.config_handle, value, none_cb,
                                       timeout)
        on_notification_set_cb(True, result, None, *on_notification_set_params)
        return result
    except (RuntimeError, BaBLEException) as err:
        if enabled:
            self.remove_callback(notification_event_uuid)
        on_notification_set_cb(
            False,
            None,
            "Error while writing notification configuration (exception={})".format(err),
            *on_notification_set_params
        )
        raise RuntimeError("Error while writing notification config (exception={})".format(err))


@asyncio.coroutine
def handle_error(self, packet, on_error):
    on_error(packet.full_status, packet.get('message', bytes.decode))


@asyncio.coroutine
def set_gatt_table(self, controller_id, services, characteristics, on_set, timeout):

    if isinstance(on_set, (tuple, list)):
        on_set_cb = on_set[0]
        on_set_params = on_set[1:]
    else:
        on_set_cb = on_set
        on_set_params = []

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("GATT table set with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            data = packet.get_dict(['controller_id'])

            on_set_cb(True, data, None, *on_set_params)
            future.set_result(data)
        else:
            error = BaBLEException(packet, "Failed to set GATT table")
            on_set_cb(False, None, error, *on_set_params)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(
        SetGATTTable,
        controller_id=controller_id,
        services=services,
        characteristics=characteristics
    )

    self.register_callback(
        request_packet.packet_uuid,
        callback=on_response_received,
        params={'future': future}
    )

    self.send_packet(request_packet)

    self.logger.debug("Waiting for setting GATT table response...")
    try:
        result = yield from asyncio.wait_for(future, timeout=timeout)
        return result
    except asyncio.TimeoutError:
        self.remove_callback(request_packet.packet_uuid)
        raise RuntimeError("Set GATT table timed out")


@asyncio.coroutine
def set_advertising(self, controller_id, enabled, uuids, name, company_id, advertising_data, scan_response, on_set,
                    timeout):

    if isinstance(on_set, (tuple, list)):
        on_set_cb = on_set[0]
        on_set_params = on_set[1:]
    else:
        on_set_cb = on_set
        on_set_params = []

    if uuids is not None:
        if not isinstance(uuids, (tuple, list)):
            uuids = [uuids]

        for i, uuid in enumerate(uuids):
            if isinstance(uuid, UUID):
                uuids[i] = uuid_to_string(uuid)
            elif isinstance(uuid, string_types):
                uuids[i] = switch_endianness_string(uuid)
            else:
                raise ValueError("UUID must be either a uuid.UUID object or a string")

    @asyncio.coroutine
    def on_response_received(packet, future):
        self.logger.debug("Set advertising response received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            data = packet.get_dict([
                'controller_id',
                'state'
            ])

            on_set_cb(True, data, None, *on_set_params)
            future.set_result(data)
        else:
            error = BaBLEException(packet, "Failed to set advertising")
            on_set_cb(False, None, error, *on_set_params)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(
        SetAdvertising,
        controller_id=controller_id,
        state=enabled,
        uuids=uuids,
        name=name,
        company_id=company_id,
        adv_manufacturer_data=advertising_data,
        scan_manufacturer_data=scan_response
    )

    self.register_callback(
        request_packet.packet_uuid,
        callback=on_response_received,
        params={'future': future}
    )

    self.send_packet(request_packet)

    self.logger.debug("Waiting for setting advertising response...")
    try:
        result = yield from asyncio.wait_for(future, timeout=timeout)
        return result
    except asyncio.TimeoutError:
        self.remove_callback(request_packet.packet_uuid)
        raise RuntimeError("Set advertising timed out")


@asyncio.coroutine
def notify(self, controller_id, connection_handle, attribute_handle, value, timeout):

    # TODO: use characteristic instead of attribute_handle

    @asyncio.coroutine
    def on_ack_received(packet, future):
        self.logger.debug("EmitNotification ack received with status={}".format(packet.status))
        self.remove_callback(packet.packet_uuid)

        if packet.status_code == StatusCode.Success:
            future.set_result(True)
        else:
            error = BaBLEException(packet, "Failed to send EmitNotification packet",
                                   connection_handle=connection_handle,
                                   attribute_handle=attribute_handle)
            future.set_exception(error)

    future = asyncio.Future()
    request_packet = Packet.build(
        EmitNotification,
        controller_id=controller_id,
        connection_handle=connection_handle,
        attribute_handle=attribute_handle,
        value=bytes(value)
    )

    self.register_callback(request_packet.packet_uuid, callback=on_ack_received, params={'future': future})

    self.send_packet(request_packet)

    try:
        result = yield from asyncio.wait_for(future, timeout=timeout)
        self.logger.debug("Notification sent")
        return result
    except asyncio.TimeoutError:
        self.remove_callback(request_packet.packet_uuid)
        raise RuntimeError("Notification timed out")


@asyncio.coroutine
def on_write_request(self, controller_id, on_write_handler):
    write_request_uuid = PacketUuid(payload_type=Payload.WritePeripheral, controller_id=controller_id)
    write_without_response_request_uuid = PacketUuid(
        payload_type=Payload.WriteWithoutResponsePeripheral,
        controller_id=controller_id
    )

    if isinstance(on_write_handler, (tuple, list)):
        on_write_handler_fn = on_write_handler[0]
        on_write_handler_params = on_write_handler[1:]
    else:
        on_write_handler_fn = on_write_handler
        on_write_handler_params = []

    @asyncio.coroutine
    def on_request(packet):
        request = packet.get_dict([
            'controller_id',
            'connection_handle',
            'attribute_handle',
            ('value', bytes)
        ])

        handled = on_write_handler_fn(request, *on_write_handler_params)

        if handled and packet.payload_type != Payload.WriteWithoutResponsePeripheral:
            self.send_packet(packet)

    self.register_callback(write_request_uuid, callback=on_request)
    self.register_callback(write_without_response_request_uuid, callback=on_request)
    self.logger.debug("On write handler registered")


@asyncio.coroutine
def on_read_request(self, controller_id, on_read_handler):
    read_request_uuid = PacketUuid(payload_type=Payload.ReadPeripheral, controller_id=controller_id)

    if isinstance(on_read_handler, (tuple, list)):
        on_read_handler_fn = on_read_handler[0]
        on_read_handler_params = on_read_handler[1:]
    else:
        on_read_handler_fn = on_read_handler
        on_read_handler_params = []

    @asyncio.coroutine
    def on_request(packet):
        request = packet.get_dict([
            'controller_id',
            'connection_handle',
            'attribute_handle'
        ])

        packet.value = on_read_handler_fn(request, *on_read_handler_params)

        if packet.value is not None:
            self.send_packet(packet)

    self.register_callback(read_request_uuid, callback=on_request)
    self.logger.debug("On read handler registered")


@asyncio.coroutine
def on_connected(self, controller_id, on_connected_handler):
    device_connected_uuid = PacketUuid(payload_type=Payload.DeviceConnected, controller_id=controller_id)

    if isinstance(on_connected_handler, (tuple, list)):
        on_connected_handler_fn = on_connected_handler[0]
        on_connected_handler_params = on_connected_handler[1:]
    else:
        on_connected_handler_fn = on_connected_handler
        on_connected_handler_params = []

    @asyncio.coroutine
    def on_device_connected(packet):
        if packet.status_code == StatusCode.Success:
            device = packet.get_dict([
                'controller_id',
                'connection_handle',
                'address',
                ('address_type', lambda value: 'public' if value == 0 else 'random')
            ])

            on_connected_handler_fn(device, *on_connected_handler_params)
        else:
            self.logger.debug("Failed connected event received: %s", packet)

    self.register_callback(device_connected_uuid, callback=on_device_connected)
    self.logger.debug("On connected handler registered")


@asyncio.coroutine
def on_disconnected(self, controller_id, on_disconnected_handler):
    device_disconnected_uuid = PacketUuid(payload_type=Payload.DeviceDisconnected, controller_id=controller_id)

    if isinstance(on_disconnected_handler, (tuple, list)):
        on_disconnected_handler_fn = on_disconnected_handler[0]
        on_disconnected_handler_params = on_disconnected_handler[1:]
    else:
        on_disconnected_handler_fn = on_disconnected_handler
        on_disconnected_handler_params = []

    @asyncio.coroutine
    def on_device_disconnected(packet):
        if packet.status_code == StatusCode.Success:
            device = packet.get_dict([
                'controller_id',
                'connection_handle',
                'reason',
                'code'
            ])

            on_disconnected_handler_fn(device, *on_disconnected_handler_params)
        else:
            self.logger.debug("Failed disconnected event received: %s", packet)

    self.register_callback(device_disconnected_uuid, callback=on_device_disconnected)
    self.logger.debug("On disconnected handler registered")
