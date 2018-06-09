import sys

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


def none_cb(success, result, failure_reason):
    pass

MAGIC_CODE = b'\xCA\xFE'
