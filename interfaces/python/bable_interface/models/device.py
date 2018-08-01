class Device(object):

    @classmethod
    def from_flatbuffers(cls, raw_device):
        return cls(
            connection_handle=raw_device.ConnectionHandle(),
            address=raw_device.Address().decode()
        )

    def __init__(self, connection_handle, address):
        self.connection_handle = connection_handle
        self.address = address

    def __str__(self):
        return self.__repr__()

    def __repr__(self):
        return "<Device connection_handle={}, address={}>".format(self.connection_handle, self.address)
