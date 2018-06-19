class Controller(object):

    def __init__(self, raw_ctrl):
        self.id = raw_ctrl.Id()
        self.address = raw_ctrl.Address()
        self.name = raw_ctrl.Name()
        self.powered = raw_ctrl.Powered()
        self.connectable = raw_ctrl.Connectable()
        self.discoverable = raw_ctrl.Discoverable()
        self.low_energy = raw_ctrl.LowEnergy()
