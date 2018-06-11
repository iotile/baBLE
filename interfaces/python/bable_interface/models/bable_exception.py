from bable_interface.flatbuffer import extract_packet
from bable_interface.BaBLE import BaBLEError, Payload, StatusCode


class BaBLEException(Exception):

    def __init__(self, packet, message=None, **kwargs):
        super(BaBLEException, self).__init__()

        if packet.PayloadType() == Payload.Payload.BaBLEError:
            error = extract_packet(packet=packet, payload_class=BaBLEError.BaBLEError, params=["name", "message"])
            self.name = error["name"]
            self.message = error["message"]
        else:
            self.name = "ResponseError"
            self.message = "Response with error status received" if message is None else message

        self.status = packet.Status()
        self.formatted_status = self.format_status(self.status)
        self.native_status = packet.NativeStatus()
        self.native_class = packet.NativeClass()
        self.controller_id = packet.ControllerId()
        self.kwargs = kwargs

    def __str__(self):
        result = "{} - {} (status={}, native_status={}, native_class={}, controller_id={}, "\
            .format(self.name, self.message, self.formatted_status, self.native_status, self.native_class, self.controller_id)

        for key, value in self.kwargs.items():
            result += "{}={}, ".format(key, value)

        return result[:-2] + ")"

    def __getattribute__(self, name):
        try:
            result = super(BaBLEException, self).__getattribute__(name)
        except AttributeError:
            result = self.kwargs[name]

        return result

    @classmethod
    def format_status(cls, status):
        if status == StatusCode.StatusCode().Success:
            return "Success"
        elif status == StatusCode.StatusCode().SocketError:
            return "SocketError"
        elif status == StatusCode.StatusCode().NotFound:
            return "NotFound"
        elif status == StatusCode.StatusCode().WrongFormat:
            return "WrongFormat"
        elif status == StatusCode.StatusCode().InvalidCommand:
            return "InvalidCommand"
        elif status == StatusCode.StatusCode().Unknown:
            return "Unknown"
        elif status == StatusCode.StatusCode().Rejected:
            return "Rejected"
        elif status == StatusCode.StatusCode().Denied:
            return "Denied"
        elif status == StatusCode.StatusCode().Cancelled:
            return "Cancelled"
        elif status == StatusCode.StatusCode().NotPowered:
            return "NotPowered"
        elif status == StatusCode.StatusCode().Failed:
            return "Failed"
        elif status == StatusCode.StatusCode().NotConnected:
            return "NotConnected"
        else:
            return "-"
