from uuid import UUID
from bable_interface.utils import string_types, string_to_uuid, uuid_to_string


class Service(object):

    @classmethod
    def from_flatbuffers(cls, raw_service):
        return cls(
            uuid={'uuid': raw_service.Uuid().decode(), 'byteorder': 'little'},
            handle=raw_service.Handle(),
            group_end_handle=raw_service.GroupEndHandle()
        )

    def __init__(self, uuid, handle, group_end_handle):
        if isinstance(uuid, UUID):
            self.uuid = uuid
        elif isinstance(uuid, string_types):
            self.uuid = string_to_uuid(uuid)
        elif isinstance(uuid, dict):
            self.uuid = string_to_uuid(uuid['uuid'], input_byteorder=uuid['byteorder'])
        else:
            raise ValueError("UUID must be either a uuid.UUID object, a dict or a string")

        self.handle = handle
        self.group_end_handle = group_end_handle

    def __str__(self):
        return self.__repr__()

    def __repr__(self):
        return "<Service uuid={}, start_handle={}, end_handle={}>"\
            .format(uuid_to_string(self.uuid), self.handle, self.group_end_handle)

    def __eq__(self, other):
        if isinstance(other, Service):
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
            'group_end_handle': self.group_end_handle
        }
