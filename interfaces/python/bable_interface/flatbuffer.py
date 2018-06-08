import flatbuffers
import sys

from .BaBLE import Packet, Payload, DeviceConnected, DeviceFound, DeviceDisconnected, NotificationReceived,\
    ControllerAdded, ControllerRemoved, BaBLEError

if sys.version_info < (3, 2):
    import struct

    def to_bytes(value, length, byteorder='big'):
        struct_str = '>' if byteorder == 'big' else '<'
        if value < 0:
            raise ValueError("Can't convert negative values to bytes.")

        if length == 1:
            struct_str += 'B'
        elif length == 2:
            struct_str += 'H'
        elif length == 4:
            struct_str += 'I'
        elif length == 8:
            struct_str += 'Q'
        else:
            raise ValueError("Invalid length.")

        return struct.pack(struct_str, value)
else:
    def to_bytes(value, length, byteorder='big'):
        return value.to_bytes(length, byteorder=byteorder)

# TODO: create a dict with PayloadType -> {payload_name, PayloadModule, PayloadClass} instead ?
PAYLOAD_IDS = {k: v for k, v in vars(Payload.Payload).items() if not k.startswith("__")}


def snake_to_camel(word):
    return ''.join(subword[:1].upper() + subword[1:] for subword in word.split('_'))


def get_packet_uuid(packet):
    module_name = None
    for payload_name, payload_id in PAYLOAD_IDS.items():
        if payload_id == packet.PayloadType():
            module_name = payload_name
            break

    if module_name is None:
        raise KeyError("Unknown payload in the given packet (payload_id={})".format(packet.PayloadType()))

    try:
        payload_class = getattr(globals()[module_name], module_name)
    except KeyError as e:
        print("Can't find module {} in flatbuffers.py".format(module_name))
        raise e
    except AttributeError as e:
        print("Can't find class {} in module {}".format(module_name, module_name))
        raise e

    payload = get_payload(packet, payload_class)

    address = get_variable(payload, "address")
    connection_handle = get_variable(payload, "connection_handle")
    attribute_handle = get_variable(payload, "attribute_handle")

    return (
        packet.PayloadType(),
        packet.ControllerId(),
        address.decode() if address is not None else address,
        connection_handle,
        attribute_handle
    )


def get_payload(packet, payload_class):
    payload = payload_class()
    payload.Init(packet.Payload().Bytes, packet.Payload().Pos)

    return payload


def get_variable(payload_instance, attribute_name, return_function=False):
    try:
        attribute_fn = getattr(payload_instance, snake_to_camel(attribute_name))

        if return_function:
            return attribute_fn

        return attribute_fn()
    except AttributeError:
        return None


def build_packet(payload_module, uuid="", controller_id=None, params=None):
    packet_name = payload_module.__name__.split(".")[-1]
    builder = flatbuffers.Builder(0)
    fb_params = {}

    if params is not None:
        for param_name, param_value in params.items():
            param_name = snake_to_camel(param_name)
            if isinstance(param_value, str):
                fb_params[param_name] = builder.CreateString(param_value)
            else:
                fb_params[param_name] = param_value

    getattr(payload_module, '{}Start'.format(packet_name))(builder)

    for param_name, param_value in fb_params.items():
        getattr(payload_module, '{}Add{}'.format(packet_name, param_name))(builder, param_value)

    payload = getattr(payload_module, '{}End'.format(packet_name))(builder)
    payload_type = getattr(Payload.Payload, packet_name)

    fb_uuid = builder.CreateString(uuid)

    Packet.PacketStart(builder)
    Packet.PacketAddUuid(builder, fb_uuid)

    if controller_id is not None:
        Packet.PacketAddControllerId(builder, controller_id)

    Packet.PacketAddPayloadType(builder, payload_type)
    Packet.PacketAddPayload(builder, payload)
    packet = Packet.PacketEnd(builder)

    builder.Finish(packet)

    buf = builder.Output()
    buf = b'\xCA\xFE' + to_bytes(len(buf), 2, byteorder='little') + buf

    return buf


def extract_packet(packet, payload_class, params=None, list_params=None, numpy_params=None):
    payload = get_payload(packet, payload_class)

    result = {}

    if params is not None:
        for param_name in params:
            result[param_name] = get_variable(payload, param_name)

    if list_params is not None:
        for param_name in list_params:
            list_length = get_variable(payload, '{}Length'.format(param_name))
            result[param_name] = []
            get_item_fn = get_variable(payload, param_name, return_function=True)

            for i in range(list_length):
                item = get_item_fn(i)
                result[param_name].append(item)

    if numpy_params is not None:
        for param_name in numpy_params:
            result[param_name] = get_variable(payload, '{}AsNumpy'.format(param_name))

    return result
