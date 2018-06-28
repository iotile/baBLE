import uuid
from bable_interface.BaBLE.StatusCode import StatusCode
from bable_interface.flatbuffer import build_packet, get_type, get_name, get_params, parse_packet, has_attribute


class PacketUuid(object):

    def __init__(self, payload_type=None, controller_id=None, address=None, connection_handle=None,
                 attribute_handle=None, uuid=None):
        self.payload_type = payload_type
        self.controller_id = controller_id
        self.address = address.lower() if address is not None else None
        self.connection_handle = connection_handle
        self.attribute_handle = attribute_handle
        self.uuid = uuid

    def __repr__(self):
        return "<PacketUuid payload_type={}, controller_id={}, address={}, connection_handle={}, attribute_handle={}," \
               " uuid={}>".format(self.payload_type, self.controller_id, self.address, self.connection_handle,
                                  self.attribute_handle, self.uuid)

    def set(self, address=None, connection_handle=None, attribute_handle=None, uuid=None):
        self.address = address.lower() if address is not None else None
        self.connection_handle = connection_handle
        self.attribute_handle = attribute_handle
        self.uuid = uuid

    def match(self, other):
        if self.uuid is not None:
            return self.uuid == other.uuid

        if self.payload_type != other.payload_type:
            return False

        if self.controller_id != other.controller_id:
            return False

        if self.address is not None:
            if other.address is None or self.address != other.address.lower():
                return False

        if self.connection_handle is not None:
            if self.connection_handle != other.connection_handle:
                return False

        if self.attribute_handle is not None:
            if self.attribute_handle != other.attribute_handle:
                return False

        return True


class Packet(object):

    @classmethod
    def build(cls, payload_module, controller_id=None, **kwargs):
        payload_type = get_type(payload_module)

        # Create base packet
        packet = cls(
            controller_id=controller_id,
            payload_type=payload_type
        )

        # Add extra params
        for name, value in kwargs.items():
            if not has_attribute(payload_type, name):
                raise KeyError("Can't create packet {} with '{}' attribute.".format(get_name(payload_type), name))

            packet.params[name] = value

        # Update uuid with params
        packet.packet_uuid.set(
            address=packet.params.get('address'),
            connection_handle=packet.params.get('connection_handle'),
            attribute_handle=packet.params.get('attribute_handle'),
            uuid=str(uuid.uuid4())
        )

        return packet

    @classmethod
    def extract(cls, raw_bytes):
        fb_packet = parse_packet(raw_bytes)

        # Extract base packet data
        packet = cls(
            controller_id=fb_packet.ControllerId(),
            payload_type=fb_packet.PayloadType(),
            native_class=fb_packet.NativeClass().decode(),
            native_status=fb_packet.NativeStatus(),
            status=fb_packet.Status()
        )

        # Extract data dependent to payload type
        for name, value in get_params(fb_packet):
            packet.params[name] = value

        # Update uuid with params
        packet.packet_uuid.set(
            address=packet.params.get('address'),
            connection_handle=packet.params.get('connection_handle'),
            attribute_handle=packet.params.get('attribute_handle'),
            uuid=fb_packet.Uuid().decode()
        )

        return packet

    def __init__(self, controller_id=None, native_class=None, native_status=None, status=None, payload_type=None):
        self.controller_id = controller_id
        self.native_class = native_class
        self.native_status = native_status
        self.status_code = status
        self.payload_type = payload_type

        self.status = self._format_status(self.status_code)
        self.packet_uuid = PacketUuid(payload_type=payload_type, controller_id=controller_id)
        self.params = {}

    def __repr__(self):
        result = "<{} controller_id={}, status={}, uuid={}, "\
            .format(get_name(self.payload_type), self.controller_id, self.full_status, self.packet_uuid.uuid)

        for key, value in self.params.items():
            result += "{}={}, ".format(key, value)

        return result[:-2] + ">"

    @property
    def full_status(self):
        return self.status, self.status_code, self.native_status, self.native_class

    def get(self, name, format_function=None):
        try:
            value = getattr(self, name)
        except AttributeError:
            value = self.params[name]

        return format_function(value) if format_function is not None else value

    def get_dict(self, requests):
        result = {}

        for request in requests:
            if isinstance(request, tuple):
                name = request[0]
                format_function = request[1]
            else:
                name = request
                format_function = None

            result[name] = self.get(name, format_function)

        return result

    def serialize(self):
        return build_packet(
            payload_type=self.payload_type,
            uuid=self.packet_uuid.uuid,
            controller_id=self.controller_id,
            params=self.params
        )

    @staticmethod
    def _format_status(status):
        if status == StatusCode.Success:
            return "Success"
        elif status == StatusCode.SocketError:
            return "SocketError"
        elif status == StatusCode.NotFound:
            return "NotFound"
        elif status == StatusCode.WrongFormat:
            return "WrongFormat"
        elif status == StatusCode.InvalidCommand:
            return "InvalidCommand"
        elif status == StatusCode.Unknown:
            return "Unknown"
        elif status == StatusCode.Rejected:
            return "Rejected"
        elif status == StatusCode.Denied:
            return "Denied"
        elif status == StatusCode.Cancelled:
            return "Cancelled"
        elif status == StatusCode.NotPowered:
            return "NotPowered"
        elif status == StatusCode.Failed:
            return "Failed"
        elif status == StatusCode.NotConnected:
            return "NotConnected"
        else:
            return "-"
