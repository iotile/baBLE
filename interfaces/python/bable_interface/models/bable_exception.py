from builtins import bytes
from bable_interface.BaBLE.Payload import Payload
from bable_interface.utils import string_types


class BaBLEException(Exception):

    def __init__(self, packet, message=None, **kwargs):
        super(BaBLEException, self).__init__()

        if packet.payload_type == Payload.BaBLEError:
            raw_message = packet.get('message')
            if type(raw_message) == bytes or type(raw_message) == bytearray:
                self.message = raw_message.decode()
            elif isinstance(raw_message, string_types):
                self.message = raw_message
            else:
                raise ValueError("Can't read message in BaBLEException: wrong type")
        else:
            self.message = message if message is not None else "Response with error status received"

        self.packet = packet
        self.kwargs = kwargs

    def __str__(self):
        return self.__repr__()

    def __repr__(self):
        result = "{} (packet={}, ".format(self.message, self.packet)

        for key, value in self.kwargs.items():
            result += "{}={}, ".format(key, value)

        return result[:-2] + ')'

    def __getattribute__(self, name):
        try:
            result = super(BaBLEException, self).__getattribute__(name)
        except AttributeError:
            result = self.kwargs[name]

        return result
