import inspect
import logging
import flatbuffers

# pylint:disable=unused-import
from .BaBLE import Packet, BaBLEError, CancelConnection, Connect, ControllerAdded, ControllerRemoved, \
    DeviceConnected, DeviceDisconnected, DeviceFound, Disconnect, Exit, GetConnectedDevices, GetControllersList, \
    GetControllerInfo, GetControllersIds, NotificationReceived, ProbeCharacteristics, ProbeServices, \
    Read, Ready, SetConnectable, SetDiscoverable, SetPowered, StartScan, StopScan, Write, WriteWithoutResponse
from .BaBLE.Payload import Payload
from .utils import to_bytes, MAGIC_CODE, snake_to_camel, camel_to_snake


PAYLOADS = {}
for payload_name, payload_type in vars(Payload).items():
    if not payload_name.startswith('__') and payload_name != 'NONE':
        try:
            payload_module = globals()[payload_name]
        except KeyError:
            logger = logging.getLogger(__name__)
            logger.error("%s not imported in %s", payload_name, __file__)
            continue

        payload_class = getattr(payload_module, payload_name)
        PAYLOADS[payload_type] = {
            'name': payload_name,
            'module': payload_module,
            'class': payload_class
        }


def get_payload_data(payload_type):
    payload_data = PAYLOADS.get(payload_type)

    if payload_data is None:
        raise ValueError("Payload type not found in PAYLOADS dict (payload_type={})".format(payload_type))

    return payload_data


def get_type(payload_module=None, payload_class=None, payload_name=None):
    key = None
    value = None

    if payload_module is not None:
        key = 'module'
        value = payload_module
    elif payload_class is not None:
        key = 'class'
        value = payload_class
    elif payload_name is not None:
        key = 'name'
        value = payload_name

    if key is None:
        raise ValueError("No parameter provided to get_type() function.")

    for payload_type, payload_data in PAYLOADS.items():
        if payload_data[key] == value:
            return payload_type

    raise ModuleNotFoundError("Given module '{}' not found in PAYLOADS dict. Can't get payload type."
                              .format(payload_module))


def has_attribute(payload_type, attribute_name):
    payload_data = get_payload_data(payload_type)
    payload_class = payload_data['class']
    try:
        getattr(payload_class, snake_to_camel(attribute_name))
        return True
    except AttributeError:
        return False


def get_name(payload_type):
    payload_data = get_payload_data(payload_type)
    return payload_data['name']


def parse_packet(raw_bytes):
    return Packet.Packet.GetRootAsPacket(raw_bytes, 0)


def get_params(fb_packet):
    payload_type = fb_packet.PayloadType()
    payload_data = get_payload_data(payload_type)

    payload_instance = payload_data['class']()
    payload_instance.Init(fb_packet.Payload().Bytes, fb_packet.Payload().Pos)

    for method_name, method_fn in inspect.getmembers(payload_instance, predicate=inspect.ismethod):
        if method_name == 'Init' or method_name.startswith('GetRootAs'):
            continue

        if method_name.endswith('Length'):
            result = []
            get_item_method_name = method_name[:-6]
            get_item_fn = getattr(payload_instance, get_item_method_name)

            list_length = method_fn()
            for i in range(list_length):
                item = get_item_fn(i)
                result.append(item)

            attribute_name = camel_to_snake(get_item_method_name)
            yield attribute_name, result

        elif method_name.endswith('AsNumpy'):
            continue

        else:
            try:
                result = method_fn()
                attribute_name = camel_to_snake(method_name)
                if isinstance(result, bytes):
                    result = result.decode()
                yield attribute_name, result
            except TypeError:
                continue


def build_packet(payload_type, uuid=None, controller_id=None, params=None):
    payload_data = get_payload_data(payload_type)
    payload_name = payload_data['name']
    payload_module = payload_data['module']

    builder = flatbuffers.Builder(0)
    fb_params = {}

    if params is not None:
        for param_name, param_value in params.items():
            param_name = snake_to_camel(param_name)

            if isinstance(param_value, str):
                fb_params[param_name] = builder.CreateString(param_value)

            elif isinstance(param_value, (tuple, list, bytes)):
                getattr(payload_module, "{}Start{}Vector".format(payload_name, param_name))(builder, len(param_value))
                for element in reversed(param_value):
                    builder.PrependByte(element)
                fb_params[param_name] = builder.EndVector(len(param_value))

            else:
                fb_params[param_name] = param_value

    getattr(payload_module, "{}Start".format(payload_name))(builder)

    for param_name, param_value in fb_params.items():
        getattr(payload_module, "{}Add{}".format(payload_name, param_name))(builder, param_value)

    payload = getattr(payload_module, "{}End".format(payload_name))(builder)

    if uuid is None:
        uuid = ""

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

    return MAGIC_CODE + to_bytes(len(buf), 2, byteorder='little') + buf
