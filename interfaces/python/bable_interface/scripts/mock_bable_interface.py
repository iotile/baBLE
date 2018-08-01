from builtins import bytes
import pytest
import bable_interface
from bable_interface.models import BaBLEException, Controller, Device, Packet
from bable_interface.BaBLE import Payload, StatusCode
from bable_interface.utils import none_cb


class MockBaBLE(bable_interface.BaBLEInterface):

    def __init__(self):
        super(MockBaBLE, self).__init__()

        self.controllers = []
        self.controllers_state = {}

        self.handlers = {
            'on_connected': {},
            'on_disconnected': {},
            'on_write_request': {},
            'on_read_request': {}
        }

        self.counters = {
            'disconnect': 0,
            'connect': 0,
            'start': 0,
            'stop': 0,
            'start_scan': 0,
            'stop_scan': 0,
            'cancel_connection': 0,
            'probe_services': 0,
            'probe_characteristics': 0,
            'list_connected_devices': 0,
            'list_controllers': 0,
            'read': 0,
            'write': 0,
            'write_without_response': 0,
            'set_notification': 0,
            'set_gatt_table': 0,
            'set_advertising': 0,
            'notify': 0,
            'on_connected': 0,
            'on_disconnected': 0,
            'on_write_request': 0,
            'on_read_request': 0
        }

    def set_controllers(self, controllers):
        self.controllers = controllers
        self.controllers_state = {
            controller.id: {
                'scanning': {},
                'connected': {},
                'connecting': {},
                'advertising': {},
                'gatt_table': {'services': [], 'characteristics': []}
            } for controller in self.controllers
        }

    def start(self, controller_id=None, on_error=None, use_path=False, exit_on_sigint=True):
        self.counters['start'] += 1
        self.on_error = on_error

        if controller_id is not None:
            self.selected_controller_id = int(controller_id)
            self.controller_locked = True

        self.started = True

    def stop(self, sync=True):
        self.counters['stop'] += 1
        self.started = False

    def _run_command(self, command_name, params, sync):
        self.counters[command_name] += 1

        if not self.started:
            raise RuntimeError("BaBLEInterface not running")

        success, result, failure_reason, callback = getattr(self, '_simulate_{}'.format(command_name))(*params)

        if sync:
            if success:
                if command_name == 'connect':
                    return self.simulate_connected_event(result['controller_id'], result['address'])
                elif command_name == 'disconnect':
                    return self.simulate_disconnected_event(result['controller_id'], result['connection_handle'])

                return result
            else:
                raise failure_reason
        elif callback is not None:
            cb, cb_params = self._process_callback(callback)
            cb(success, result, failure_reason, *cb_params)

    @staticmethod
    def _process_callback(callback):
        if isinstance(callback, (tuple, list)):
            cb = callback[0]
            cb_params = callback[1:]
        else:
            cb = callback
            cb_params = []

        return cb, cb_params

    @staticmethod
    def _generate_error(controller_id, message, native_class='HCI', native_status=0x0c,
                        status=StatusCode.StatusCode.Denied, **kwargs):
        return BaBLEException(
            packet=Packet(
                payload_type=Payload.Payload.BaBLEError,
                controller_id=controller_id,
                native_class=native_class,
                native_status=native_status,
                status=status,
                message=message
            ),
            **kwargs
        )

    def _verify_connected(self, controller_id, connection_handle=None, address=None):
        if address is not None and connection_handle is not None:
            raise ValueError("Need either connection handle or address to verify connection")

        if connection_handle is not None:
            return self.controllers_state[controller_id]['connected'].get(connection_handle, False)

        if address is not None:
            for connection in self.controllers_state[controller_id]['connected'].values():
                if connection['device'].address == address:
                    return True
            return False

        raise ValueError("Need either connection handle or address to verify connection")

    #### Simulate events ####
    def simulate_device_found(self, controller_id, device_info):
        if not self.controllers_state[controller_id]['scanning']:
            raise ValueError("Controller not scanning, controller_state={}"
                             .format(self.controllers_state[controller_id]))

        scan_info = self.controllers_state[controller_id]['scanning']
        if not scan_info['active_scan'] and device_info['type'] == 0x04:
            raise ValueError("Controller currently running passive scan: can't receive scan response")

        on_device_found_cb, on_device_found_params = scan_info['on_device_found']
        on_device_found_cb(True, device_info, None, *on_device_found_params)

    def simulate_connected_event(self, controller_id, address, address_type='random'):
        if address not in self.controllers_state[controller_id]['connecting']:
            self.controllers_state[controller_id]['connecting'].update({
                address: {
                    'address_type': address_type,
                    'on_connected': self._process_callback(none_cb),
                    'on_disconnected': self._process_callback(none_cb)
                }
            })

        if self._verify_connected(controller_id, address=address):
            raise ValueError("Device with given address already connected to the given controller, "
                             "address=%s, controller_id=%s".format(address, controller_id))

        connected_devices = self.controllers_state[controller_id]['connected']
        connecting_devices = self.controllers_state[controller_id]['connecting']

        connection_handle = max(connected_devices.keys()) + 1 if len(connected_devices) > 0 else 1
        connected_devices[connection_handle] = {
            'device': Device(connection_handle, address),
            'on_disconnected': connecting_devices[address]['on_disconnected'],
            'on_notification_received': self._process_callback(none_cb)
        }

        address_type = connecting_devices[address]['address_type']
        on_connected_cb, on_connected_params = connecting_devices[address]['on_connected']
        del connecting_devices[address]
        self.controllers_state[controller_id]['advertising'].clear()

        result = {
            'controller_id': controller_id,
            'connection_handle': connection_handle,
            'address': address,
            'address_type': address_type
        }
        on_connected_cb(True, result, None, *on_connected_params)

        for key in [None, controller_id]:
            if key in self.handlers['on_connected']:
                on_connected_handler_fn, on_connected_handler_params = self.handlers['on_connected'][key]
                on_connected_handler_fn(result, *on_connected_handler_params)

        return result

    def simulate_disconnected_event(self, controller_id, connection_handle, code=0x16):
        if not self._verify_connected(controller_id, connection_handle=connection_handle):
            raise ValueError("Connection handle not found in connected devices list, controller_state={}"
                             .format(self.controllers_state[controller_id]))

        device = self.controllers_state[controller_id]['connected'][connection_handle]
        on_disconnected_cb, on_disconnected_params = device['on_disconnected']

        del self.controllers_state[controller_id]['connected'][connection_handle]

        result = {
            'controller_id': controller_id,
            'connection_handle': connection_handle,
            'reason': "Connection terminated by local host",
            'code': code
        }
        on_disconnected_cb(True, result, None, *on_disconnected_params)

        for key in [None, controller_id]:
            if key in self.handlers['on_disconnected']:
                on_disconnected_handler_fn, on_disconnected_handler_params = self.handlers['on_disconnected'][key]
                on_disconnected_handler_fn(result, *on_disconnected_handler_params)

        return result

    def simulate_write_request(self, controller_id, connection_handle, attribute_handle, value):
        if not self.handlers['on_write_request']:
            raise RuntimeError("No on_write_request handler registered")

        if not self._verify_connected(controller_id, connection_handle=connection_handle):
            raise ValueError("Connection handle not found in connected devices list, controller_state={}"
                             .format(self.controllers_state[controller_id]))

        attribute_found = False
        write_allowed = False
        for characteristic in self.controllers_state[controller_id]['gatt_table']['characteristics']:
            if characteristic.value_handle == attribute_handle or characteristic.config_handle == attribute_handle:
                attribute_found = True
                write_allowed = characteristic.properties['write'] or characteristic.config_handle == attribute_handle
                break

        if not attribute_found:
            raise ValueError("Attribute not found in GATT table, attribute_handle={}".format(attribute_handle))

        if not write_allowed:
            raise ValueError("Attribute can't be written (not allowed), attribute_handle={}".format(attribute_handle))

        request = {
            'controller_id': controller_id,
            'connection_handle': connection_handle,
            'attribute_handle': attribute_handle,
            'value': bytes(value)
        }

        handled = False
        for key in [None, controller_id]:
            if key in self.handlers['on_write_request']:
                on_write_request_handler_fn, on_write_request_handler_params = self.handlers['on_write_request'][key]
                handled = on_write_request_handler_fn(request, *on_write_request_handler_params)

        return handled

    def simulate_read_request(self, controller_id, connection_handle, attribute_handle):
        if self.handlers['on_read_request'] is None:
            raise RuntimeError("No on_read_request handler registered")

        if not self._verify_connected(controller_id, connection_handle=connection_handle):
            raise ValueError("Connection handle not found in connected devices list, controller_state={}"
                             .format(self.controllers_state[controller_id]))

        attribute_found = False
        read_allowed = False
        for characteristic in self.controllers_state[controller_id]['gatt_table']['characteristics']:
            if characteristic.value_handle == attribute_handle or characteristic.config_handle == attribute_handle:
                attribute_found = True
                read_allowed = characteristic.properties['read'] or characteristic.config_handle == attribute_handle
                break

        if not attribute_found:
            raise ValueError("Attribute not found in GATT table, attribute_handle={}".format(attribute_handle))

        if not read_allowed:
            raise ValueError("Attribute can't be read (not allowed), attribute_handle={}".format(attribute_handle))

        request = {
            'controller_id': controller_id,
            'connection_handle': connection_handle,
            'attribute_handle': attribute_handle
        }

        result = None
        for key in [None, controller_id]:
            if key in self.handlers['on_write_request']:
                on_read_request_handler_fn, on_read_request_handler_params = self.handlers['on_read_request'][key]
                result = on_read_request_handler_fn(request, *on_read_request_handler_params)

        return result

    #### Simulate response to bable_interface commands ####
    def _simulate_connect(self, controller_id, address, address_type, connection_interval, on_connected,
                          on_disconnected, timeout):
        success = True
        failure_reason = None

        if address_type not in ['public', 'random']:
            success = False
            failure_reason = self._generate_error(controller_id, "Wrong address type", address_type=address_type)

        if success:
            if self._verify_connected(controller_id, address=address):
                success = False
                failure_reason = self._generate_error(controller_id, "Device already connected", address=address)

        if success:
            if address in self.controllers_state[controller_id]['connecting']:
                success = False
                failure_reason = self._generate_error(controller_id, "Device already connecting", address=address)
            else:
                self.controllers_state[controller_id]['connecting'].update({
                    address: {
                        'address_type': address_type,
                        'on_connected': self._process_callback(on_connected),
                        'on_disconnected': self._process_callback(on_disconnected),
                        'on_notification_received': self._process_callback(none_cb)
                    }
                })

        return success, {'controller_id': controller_id, 'address': address}, failure_reason, None

    def _simulate_start_scan(self, controller_id, active_scan, on_device_found, on_scan_started, timeout):
        success = True
        failure_reason = None

        if self.controllers_state[controller_id]['scanning']:
            success = False
            failure_reason = self._generate_error(controller_id, "Scan already started")
        else:
            self.controllers_state[controller_id]['scanning'].update({
                'active_scan': active_scan,
                'on_device_found': self._process_callback(on_device_found)
            })

        return success, None, failure_reason, on_scan_started

    def _simulate_stop_scan(self, controller_id, on_scan_stopped, timeout):
        success = True
        failure_reason = None

        if not self.controllers_state[controller_id]['scanning']:
            success = False
            failure_reason = self._generate_error(controller_id, "Scan already stopped")
        else:
            self.controllers_state[controller_id]['scanning'].clear()

        return success, None, failure_reason, on_scan_stopped

    def _simulate_disconnect(self, controller_id, connection_handle, callback, timeout):
        success = True
        result = {'controller_id': controller_id, 'connection_handle': connection_handle}
        failure_reason = None

        if not self._verify_connected(controller_id, connection_handle=connection_handle):
            success = False
            failure_reason = self._generate_error(
                controller_id,
                "Connection handle not found",
                connection_handle=connection_handle
            )

        return success, result, failure_reason, callback

    def _simulate_cancel_connection(self, controller_id, on_connection_cancelled, timeout):
        self.controllers_state[controller_id]['connecting'].clear()
        return True, True, None, on_connection_cancelled

    def _simulate_list_connected_devices(self, controller_id, timeout):
        connected_devices = [connection['device'] for connection in self.controllers_state[controller_id]['connected'].values()]
        return True, connected_devices, None, None

    def _simulate_list_controllers(self, timeout):
        return True, self.controllers, None, None

    def _simulate_set_advertising(self, controller_id, enabled, uuids, name, company_id, advertising_data,
                                  scan_response, on_set, timeout):
        success = True
        failure_reason = None

        advertising = self.controllers_state[controller_id]['advertising']
        if (enabled and advertising) or (not enabled and not advertising):
            success = False
            failure_reason = self._generate_error(
                controller_id,
                "Already advertising" if enabled else "Already stopped advertising"
            )
        elif enabled:
            advertising.update({
                'uuids': uuids,
                'name': name,
                'company_id': company_id,
                'advertising_data': advertising_data,
                'scan_response': scan_response
            })
        else:
            advertising.clear()

        return success, None, failure_reason, on_set

    def _simulate_on_write_request(self, controller_id, on_write_handler):
        self.handlers['on_write_request'][controller_id] = self._process_callback(on_write_handler)
        return True, None, None, None

    def _simulate_on_read_request(self, controller_id, on_read_handler):
        self.handlers['on_read_request'][controller_id] = self._process_callback(on_read_handler)
        return True, None, None, None

    def _simulate_on_connected(self, controller_id, on_connected_handler):
        self.handlers['on_connected'][controller_id] = self._process_callback(on_connected_handler)
        return True, None, None, None

    def _simulate_on_disconnected(self, controller_id, on_disconnected_handler):
        self.handlers['on_disconnected'][controller_id] = self._process_callback(on_disconnected_handler)
        return True, None, None, None

    def _simulate_set_gatt_table(self, controller_id, services, characteristics, on_set, timeout):
        gatt_table = self.controllers_state[controller_id]['gatt_table']
        gatt_table['services'] = services
        gatt_table['characteristics'] = characteristics
        return True, {'controller_id': controller_id}, None, on_set

    def _simulate_notify(self, controller_id, connection_handle, attribute_handle, value, timeout):
        success = True
        result = True
        failure_reason = None

        if not self._verify_connected(controller_id, connection_handle=connection_handle):
            success = False
            result = None
            failure_reason = self._generate_error(
                controller_id,
                "Connection handle not found in devices connected list",
                connection_handle=connection_handle
            )

        attribute_found = False
        notify_allowed = False
        for characteristic in self.controllers_state[controller_id]['gatt_table']['characteristics']:
            if characteristic.value_handle == attribute_handle or characteristic.config_handle == attribute_handle:
                attribute_found = True
                notify_allowed = characteristic.properties['notify']
                break

        if not attribute_found or not notify_allowed:
            success = False
            result = None
            failure_reason = self._generate_error(
                controller_id,
                "Attribute not found in GATT table" if notify_allowed else "Attribute can't be notified (not allowed)",
                attribute_handle=attribute_handle
            )

        notification = {
            'controller_id': controller_id,
            'connection_handle': connection_handle,
            'attribute_handle': attribute_handle,
            'value': bytes(value)
        }

        connection = self.controllers_state[controller_id]['connected'][connection_handle]
        on_notification_received_cb, on_notification_received_params = connection['on_notification_received']
        on_notification_received_cb(True, notification, None, *on_notification_received_params)

        return success, result, failure_reason, None

    def _simulate_probe_services(self, controller_id, connection_handle, on_services_probed, timeout):
        services = self.controllers_state[controller_id]['gatt_table']['services']
        result = {
            'controller_id': controller_id,
            'connection_handle': connection_handle,
            'services': services
        }
        return True, result, None, on_services_probed

    def _simulate_probe_characteristics(self, controller_id, connection_handle, start_handle, end_handle,
                                        on_characteristics_probed, timeout):
        gatt_table = self.controllers_state[controller_id]['gatt_table']
        characteristics = [characteristic for characteristic in gatt_table['characteristics']
                           if start_handle < characteristic.handle < end_handle]
        result = {
            'controller_id': controller_id,
            'connection_handle': connection_handle,
            'characteristics': characteristics
        }
        return True, result, None, on_characteristics_probed

    def _simulate_read(self, controller_id, connection_handle, attribute_handle, on_read, timeout):
        try:
            result = self.simulate_read_request(controller_id, connection_handle, attribute_handle)
            return True, result, None, on_read
        except Exception as err:
            return False, None, err, on_read

    def _simulate_write(self, controller_id, connection_handle, attribute_handle, value, on_written, timeout):
        try:
            result = self.simulate_write_request(controller_id, connection_handle, attribute_handle, value)
            return True, result, None, on_written
        except Exception as err:
            return False, None, err, on_written

    def _simulate_write_without_response(self, controller_id, connection_handle, attribute_handle, value, timeout):
        try:
            result = self.simulate_write_request(controller_id, connection_handle, attribute_handle, value)
            return True, result, None, None
        except Exception as err:
            return False, None, err, None

    def _simulate_set_notification(self, controller_id, enabled, connection_handle, characteristic, on_notification_set,
                                   on_notification_received, timeout):
        success = True
        result = None
        failure_reason = None

        if not self._verify_connected(controller_id, connection_handle=connection_handle):
            success = False
            failure_reason = self._generate_error(
                controller_id,
                "Connection handle not found in devices connected list",
            )
        else:
            attribute_handle = characteristic.config_handle

            if enabled:
                value = bytes(b'\x00\x01')
            else:
                value = bytes(b'\x00\x00')

            try:
                result = self.simulate_write_request(controller_id, connection_handle, attribute_handle, value)
                connection = self.controllers_state[controller_id]['connected'][connection_handle]
                connection['on_notification_received'] = self._process_callback(on_notification_received)
            except Exception as err:
                success = False
                failure_reason = err

        return success, result, failure_reason, on_notification_set


@pytest.fixture(scope='function')
def mock_bable(monkeypatch):
    """ Mock the BaBLEInterface class with some controllers inside. """
    mocked_bable = MockBaBLE()
    mocked_bable.set_controllers([
        Controller(0, '11:22:33:44:55:66', '#0'),
        Controller(1, '22:33:44:55:66:11', '#1', settings={'powered': True, 'low_energy': True}),
        Controller(2, '33:44:55:66:11:22', '#2', settings={'powered': True})
    ])

    monkeypatch.setattr(bable_interface, 'BaBLEInterface', lambda: mocked_bable)

    return mocked_bable


@pytest.fixture(scope='function')
def mock_bable_no_ctrl(monkeypatch):
    """ Mock the BaBLEInterface class without any controller. """
    mocked_bable = MockBaBLE()

    monkeypatch.setattr(bable_interface, 'BaBLEInterface', lambda: mocked_bable)

    return mocked_bable
