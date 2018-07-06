class Characteristic(object):

    def __init__(self, raw_characteristic):
        self.uuid = raw_characteristic.Uuid().decode()

        self.handle = raw_characteristic.Handle()
        self.value_handle = raw_characteristic.ValueHandle()
        self.config_handle = raw_characteristic.ConfigHandle()

        self.properties = {
            'indicate': raw_characteristic.Indicate(),
            'notify': raw_characteristic.Notify(),
            'read': raw_characteristic.Read(),
            'write': raw_characteristic.Write(),
            'broadcast': raw_characteristic.Broadcast()
        }

        self.notification_enabled = raw_characteristic.NotificationEnabled()
        self.indication_enabled = raw_characteristic.IndicationEnabled()

    def __str__(self):
        return self.__repr__()

    def __repr__(self):
        return "<Characteristic uuid={}, handle={}, value_handle={}, config_handle={}, properties={}"\
            .format(self.uuid, self.handle, self.value_handle, self.config_handle, self.properties)
