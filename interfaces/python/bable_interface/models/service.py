from uuid import UUID
from bable_interface.utils import string_types, string_to_uuid, uuid_to_string


class Service(object):

    @classmethod
    def from_flatbuffers(cls, raw_service):
        return cls(
            uuid=raw_service.Uuid().decode(),
            handle=raw_service.Handle(),
            group_end_handle=raw_service.GroupEndHandle()
        )

    def __init__(self, uuid, handle, group_end_handle):
        if isinstance(uuid, UUID):
            self.uuid = uuid
        elif isinstance(uuid, string_types):
            self.uuid = string_to_uuid(uuid)
        else:
            raise ValueError("UUID must be either a uuid.UUID object or a string")

        self.handle = handle
        self.group_end_handle = group_end_handle

    def __str__(self):
        return self.__repr__()

    def __repr__(self):
        return "<Service uuid={}, start_handle={}, end_handle={}>"\
            .format(self.uuid, self.handle, self.group_end_handle)

    def to_dict(self):
        return {
            'uuid': uuid_to_string(self.uuid),
            'handle': self.handle,
            'group_end_handle': self.group_end_handle
        }
