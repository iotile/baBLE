class Characteristic(object):

    def __init__(self, raw_characteristic):
        self.uuid = raw_characteristic.Uuid().decode()

        self.handle = raw_characteristic.Handle()
        self.value_handle = raw_characteristic.ValueHandle()
        self.config_handle = raw_characteristic.ConfigHandle()

        self.indicate = raw_characteristic.Indicate()
        self.notify = raw_characteristic.Notify()
        self.read = raw_characteristic.Read()
        self.write = raw_characteristic.Write()
        self.broadcast = raw_characteristic.Broadcast()

        self.notification_enabled = raw_characteristic.NotificationEnabled()
        self.indication_enabled = raw_characteristic.IndicationEnabled()

    def __repr__(self):
        result = "<Characteristic uuid={}, handle={}, value_handle={}, config_handle={}, properties=["\
            .format(self.uuid, self.handle, self.value_handle, self.config_handle)

        if self.indicate:
            result += "indicate, "
        if self.notify:
            result += "notify, "
        if self.read:
            result += "read, "
        if self.write:
            result += "write, "
        if self.broadcast:
            result += "broadcast, "

        return result[:-2] + "]>"
