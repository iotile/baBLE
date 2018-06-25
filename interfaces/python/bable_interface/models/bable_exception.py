from bable_interface.BaBLE.Payload import Payload


class BaBLEException(Exception):

    def __init__(self, packet, message=None, **kwargs):
        super(BaBLEException, self).__init__()

        if packet.payload_type == Payload.BaBLEError:
            self.message = packet.message
        else:
            self.message = message if message is not None else "Response with error status received"

        self.packet = packet
        self.kwargs = kwargs

    def __str__(self):
        result = "{} (packet={}, ".format(self.message, self.packet)

        for key, value in self.kwargs.items():
            result += "{}={}, ".format(key, value)

        return result[:-2] + ")"

    def __getattribute__(self, name):
        try:
            result = super(BaBLEException, self).__getattribute__(name)
        except AttributeError:
            result = self.kwargs[name]

        return result
