class Device(object):

    def __init__(self, raw_device):
        self.connection_handle = raw_device.ConnectionHandle()
        self.address = raw_device.Address().decode()

    def __str__(self):
        return self.__repr__()

    def __repr__(self):
        return "<Device connection_handle={}, address={}>" \
            .format(self.connection_handle, self.address)
