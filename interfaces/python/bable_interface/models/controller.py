class Controller(object):

    @classmethod
    def from_flatbuffers(cls, raw_ctrl):
        return cls(
            id=raw_ctrl.Id(),
            address=raw_ctrl.Address().decode(),
            name=raw_ctrl.Name().decode(),
            settings={
                'powered': raw_ctrl.Powered(),
                'connectable': raw_ctrl.Connectable(),
                'discoverable': raw_ctrl.Discoverable(),
                'low_energy': raw_ctrl.LowEnergy(),
                'advertising': raw_ctrl.Advertising(),
            }
        )

    def __init__(self, id, address, name=None, settings=None):
        self.id = id
        self.address = address
        self.name = name

        if settings is None:
            settings = {}

        self.powered = settings.get('powered', False)
        self.connectable = settings.get('connectable', False)
        self.discoverable = settings.get('discoverable', False)
        self.low_energy = settings.get('low_energy', False)
        self.advertising = settings.get('advertising', False)

    def __str__(self):
        return self.__repr__()

    def __repr__(self):
        return "<Controller id={}, address={}, name={}>".format(self.id, self.address, self.name)
