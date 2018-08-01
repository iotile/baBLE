from uuid import UUID
from bable_interface.utils import string_types, string_to_uuid, uuid_to_string


class Characteristic(object):

    @classmethod
    def from_flatbuffers(cls, raw_characteristic):
        return cls(
            uuid={'uuid': raw_characteristic.Uuid().decode(), 'byteorder': 'little'},
            handle=raw_characteristic.Handle(),
            value_handle=raw_characteristic.ValueHandle(),
            config_handle=raw_characteristic.ConfigHandle(),
            indicate=raw_characteristic.Indicate(),
            notify=raw_characteristic.Notify(),
            read=raw_characteristic.Read(),
            write=raw_characteristic.Write(),
            broadcast=raw_characteristic.Broadcast(),
            notification_enabled=raw_characteristic.NotificationEnabled(),
            indication_enabled=raw_characteristic.IndicationEnabled()
        )

    def __init__(self, uuid, handle, value_handle, config_handle=None, indicate=False, notify=False, read=False,
                 write=False, broadcast=False, notification_enabled=False, indication_enabled=False, const_value=None):
        if isinstance(uuid, UUID):
            self.uuid = uuid
        elif isinstance(uuid, string_types):
            self.uuid = string_to_uuid(uuid)
        elif isinstance(uuid, dict):
            self.uuid = string_to_uuid(uuid['uuid'], input_byteorder=uuid['byteorder'])
        else:
            raise ValueError("UUID must be either a uuid.UUID object, a dict or a string")

        self.handle = handle
        self.value_handle = value_handle
        self.config_handle = config_handle

        self.properties = {
            'indicate': indicate,
            'notify': notify,
            'read': read,
            'write': write,
            'broadcast': broadcast
        }

        self.notification_enabled = notification_enabled
        self.indication_enabled = indication_enabled

        self.const_value = const_value

    def __str__(self):
        return self.__repr__()

    def __repr__(self):
        return "<Characteristic uuid={}, handle={}, value_handle={}, config_handle={}, properties={}"\
            .format(uuid_to_string(self.uuid), self.handle, self.value_handle, self.config_handle, self.properties)

    def __eq__(self, other):
        if isinstance(other, Characteristic):
            return self.uuid == other.uuid
        return NotImplemented

    def __ne__(self, other):
        equal = self.__eq__(other)
        if equal is not NotImplemented:
            return not equal
        return NotImplemented

    def __hash__(self):
        return self.uuid.__hash__()

    def to_dict(self):
        return {
            'uuid': uuid_to_string(self.uuid),
            'handle': self.handle,
            'value_handle': self.value_handle,
            'config_handle': self.config_handle,
            'indicate': self.properties['indicate'],
            'notify': self.properties['notify'],
            'read': self.properties['read'],
            'write': self.properties['write'],
            'broadcast': self.properties['broadcast'],
            'notification_enabled': self.notification_enabled,
            'indication_enabled': self.indication_enabled,
            'const_value': self.const_value
        }
