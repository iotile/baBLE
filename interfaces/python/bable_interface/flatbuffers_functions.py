import inspect
import logging
import flatbuffers
from builtins import int

# pylint:disable=unused-import
from .BaBLE import Packet, Payload, Model, BaBLEError, CancelConnection, Characteristic, Connect, Controller, \
    ControllerAdded, ControllerRemoved, Device, DeviceConnected, DeviceDisconnected, DeviceFound, Disconnect, \
    EmitNotification, Exit, GetConnectedDevices, GetControllersList, GetControllerInfo, GetControllersIds, \
    NotificationReceived, ProbeCharacteristics, ProbeServices, ReadCentral, ReadPeripheral, Ready, Service, \
    SetAdvertising, SetConnectable, SetDiscoverable, SetGATTTable,  SetPowered, StartScan, StopScan, WriteCentral, \
    WritePeripheral, WriteWithoutResponseCentral, WriteWithoutResponsePeripheral
from .utils import to_bytes, MAGIC_CODE, snake_to_camel, camel_to_snake, string_types


def get_modules_info(list_class):
    result = {}
    for name, type in vars(list_class).items():
        if not name.startswith('__') and name != 'NONE':
            try:
                payload_module = globals()[name]
            except KeyError:
                logger = logging.getLogger(__name__)
                logger.error("%s not imported in %s", name, __file__)
                continue

            payload_class = getattr(payload_module, name)
            result[type] = {
                'name': name,
                'module': payload_module,
                'class': payload_class
            }
    return result


PROTOCOL = {
    'payloads': get_modules_info(Payload.Payload),
    'models': get_modules_info(Model.Model)
}


def get_data(type, key=None, category='payloads'):
    data = PROTOCOL[category].get(type)

    if data is None:
        raise ValueError("Type not found in {} modules (type={})".format(category, type))

    return data[key] if key is not None else data


def get_type(from_module=None, from_class=None, from_name=None, category='payloads'):
    key = None
    value = None

    if from_module is not None:
        key = 'module'
        value = from_module
    elif from_class is not None:
        key = 'class'
        value = from_class
    elif from_name is not None:
        key = 'name'
        value = from_name

    if key is None:
        raise ValueError("No parameter provided to get_type() function.")

    for payload_type, payload_data in PROTOCOL[category].items():
        if payload_data[key] == value:
            return payload_type

    raise ImportError("Data not found in {} modules. Can't get type. (module={}, class={}, name={})"
                      .format(category, from_module, from_class, from_name))


def has_attribute(type, attribute_name, category='payloads'):
    data = get_data(type, category=category)
    cls = data['class']
    try:
        getattr(cls, snake_to_camel(attribute_name))
        return True
    except AttributeError:
        return False


def parse_packet(raw_bytes):
    return Packet.Packet.GetRootAsPacket(raw_bytes, 0)


def get_params(fb_packet):
    payload_type = fb_packet.PayloadType()
    payload_data = get_data(payload_type)

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
                yield attribute_name, result
            except TypeError:
                continue


def build_payload(builder, payload_name, payload_module, params=None):
    fb_params = {}
    if params is not None:
        for param_name, param_value in params.items():
            if param_value is None:
                continue

            param_name = snake_to_camel(param_name)

            if isinstance(param_value, string_types):
                fb_params[param_name] = builder.CreateString(param_value)

            elif isinstance(param_value, (tuple, list, bytes)):
                if len(param_value) == 0:
                    continue

                type_first_element = type(param_value[0])
                if not all(type(v) is type_first_element for v in param_value):
                    raise ValueError("Can't build Flatbuffers packet with heterogeneous list.")

                if param_value[0] is None:
                    continue

                if isinstance(param_value[0], string_types):
                    prepend_fn = builder.PrependUOffsetTRelative
                    elements = [builder.CreateString(v) for v in param_value]
                elif isinstance(param_value[0], (int, float, bool)):
                    prepend_fn = builder.PrependByte
                    elements = param_value
                else:
                    prepend_fn = builder.PrependUOffsetTRelative
                    model_type = get_type(from_name=param_value[0].__class__.__name__, category='models')
                    model_name = get_data(model_type, key='name', category='models')
                    model_module = get_data(model_type, key='module', category='models')
                    elements = [
                        build_payload(
                            builder=builder,
                            payload_name=model_name,
                            payload_module=model_module,
                            params=v.to_dict()
                        )
                        for v in param_value
                    ]

                getattr(payload_module, "{}Start{}Vector".format(payload_name, param_name))(builder, len(param_value))
                for element in reversed(elements):
                    prepend_fn(element)
                fb_params[param_name] = builder.EndVector(len(elements))

            else:
                fb_params[param_name] = param_value

    getattr(payload_module, "{}Start".format(payload_name))(builder)

    for param_name, param_value in fb_params.items():
        getattr(payload_module, "{}Add{}".format(payload_name, param_name))(builder, param_value)

    return getattr(payload_module, "{}End".format(payload_name))(builder)


def build_packet(payload_type, uuid=None, controller_id=None, params=None):
    payload_name = get_data(payload_type, key='name')
    payload_module = get_data(payload_type, key='module')

    builder = flatbuffers.Builder(0)

    payload = build_payload(builder=builder, payload_name=payload_name, payload_module=payload_module, params=params)

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
