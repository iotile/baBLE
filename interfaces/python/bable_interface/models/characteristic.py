class Characteristic(object):

    def __init__(self, raw_characteristic):
        self.handle = raw_characteristic.Handle()
        self.value_handle = raw_characteristic.ValueHandle()
        self.config_handle = raw_characteristic.ConfigHandle()

        self.notification_enabled = raw_characteristic.NotificationEnabled()
        self.indication_enabled = raw_characteristic.IndicationEnabled()

        self.uuid = raw_characteristic.Uuid().decode()

        self.indicate = raw_characteristic.Indicate()
        self.notify = raw_characteristic.Notify()
        self.read = raw_characteristic.Read()
        self.write = raw_characteristic.Write()
        self.broadcast = raw_characteristic.Broadcast()

# TODO: add str
