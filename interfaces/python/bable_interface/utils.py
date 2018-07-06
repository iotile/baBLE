import re
import sys

FIRST_CAP_REGEX = re.compile('(.)([A-Z][a-z]+)')
ALL_CAP_REGEX = re.compile('([a-z0-9])([A-Z])')

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


MAGIC_CODE = b'\xCA\xFE'
