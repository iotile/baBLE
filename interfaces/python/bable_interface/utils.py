import re
import sys
from uuid import UUID

FIRST_CAP_REGEX = re.compile('(.)([A-Z][a-z]+)')
ALL_CAP_REGEX = re.compile('([a-z0-9])([A-Z])')

MAGIC_CODE = b'\xCA\xFE'
BASE_UUID_BT = UUID('00000000-0000-1000-8000-00805f9b34fb')


if sys.version_info < (3, 2):
    import struct

    def to_bytes(value, length, byteorder='big'):
        if not isinstance(value, int):
            raise AttributeError("Cannot convert argument to integer")

        if value < 0:
            raise OverflowError("Cannot convert negative values to bytes.")

        struct_str = '>' if byteorder == 'big' else '<'

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

        try:
            return struct.pack(struct_str, value)
        except struct.error:
            raise OverflowError("Value too big to convert")
else:
    def to_bytes(value, length, byteorder='big'):
        return value.to_bytes(length, byteorder=byteorder)


def none_cb(success, result, failure_reason):
    pass


def snake_to_camel(word):
    return ''.join(subword[:1].upper() + subword[1:] for subword in word.split('_'))


def camel_to_snake(word):
    word = FIRST_CAP_REGEX.sub(r'\1_\2', word)
    return ALL_CAP_REGEX.sub(r'\1_\2', word).lower()


if sys.version_info < (3, 0):
    string_types = (str, unicode)
else:
    string_types = str


def string_to_uuid(string, input_byteorder='big'):
    if len(string) == 0:
        return None

    string = string.replace('-', '')
    if input_byteorder == 'little':
        string = switch_endianness_string(string)

    if len(string) not in [4, 32]:
        raise ValueError("Invalid uuid length (is not 2 or 16 bytes) uuid={}".format(string))

    if len(string) != 32:
        string = string.zfill(8) + BASE_UUID_BT.hex[8:]
        return UUID(string)
    else:
        return UUID(string)


def uuid_to_string(uuid, output_byteorder='little'):
    if uuid is None:
        return ''

    if uuid.hex[8:] == BASE_UUID_BT.hex[8:]:
        # 2 bytes UUID expanded with BASE_UUID_BT
        uuid2 = uuid.hex[4:8]
        if output_byteorder == 'little':
            uuid2 = switch_endianness_string(uuid2)
        return uuid2
    else:
        # 16 bytes UUID
        if output_byteorder == 'little':
            uuid16 = switch_endianness_string(uuid.hex)
        else:
            uuid16 = uuid.hex
        return uuid16


def switch_endianness_string(be_string):
    return "".join(reversed([be_string[i:i+2] for i in range(0, len(be_string), 2)]))
