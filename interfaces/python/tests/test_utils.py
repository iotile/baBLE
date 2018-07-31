import pytest
from uuid import UUID
from bable_interface.utils import to_bytes, camel_to_snake, snake_to_camel, string_to_uuid, uuid_to_string


def test_to_bytes():
    """ Test function to transform a number into bytes. """
    # Test if byteorder is correct
    assert to_bytes(18062015, 4, byteorder='little') == b'\xbf\x9a\x13\x01'
    assert to_bytes(18062015, 4, byteorder='big') == b'\x01\x13\x9a\xbf'

    # Test with not enough bytes to write the number
    with pytest.raises(OverflowError):
        to_bytes(18062015, 2, byteorder='little')

    # Test with negative number
    with pytest.raises(OverflowError):
        to_bytes(-18062015, 4, byteorder='little')

    # Test with string
    with pytest.raises(AttributeError):
        to_bytes("18062015", 4, byteorder='little')

    # Test with float
    with pytest.raises(AttributeError):
        to_bytes(18062015.5, 4, byteorder='little')


def test_camel_to_snake():
    """ Test function to transform a camel cased string into a snake cased string"""
    # Normal case
    assert camel_to_snake("CamelCase") == "camel_case"

    # Case with an abbreviation already capitalized (here HTTP)
    assert camel_to_snake("CamelHTTPCase") == "camel_http_case"

    # Case with string already camel cased
    assert camel_to_snake("camel_case") == "camel_case"


def test_snake_to_camel():
    """ Test function to transform a snake cased string into a camel cased string"""
    # Normal case
    assert snake_to_camel("snake_case") == "SnakeCase"

    # Case with an abbreviation already capitalized (here HTTP)
    assert snake_to_camel("snake_HTTP_case") == "SnakeHTTPCase"

    # Case with string already snake cased
    assert snake_to_camel("SnakeCase") == "SnakeCase"


def test_string_to_uuid():
    """ Test to convert a string into a uuid. """
    # Test with 2-byte UUID (Big Endian)
    string = '1234'
    uuid = string_to_uuid(string)
    assert uuid == UUID('00001234-0000-1000-8000-00805f9b34fb')

    # Test with 2-byte UUID (Little Endian)
    string = '3412'
    uuid = string_to_uuid(string, input_byteorder='little')
    assert uuid == UUID('00001234-0000-1000-8000-00805f9b34fb')

    # Test with 16-byte UUID (Big Endian)
    string = '12345678-1234-1234-1234-123456789112'
    uuid = string_to_uuid(string)
    assert uuid == UUID('12345678-1234-1234-1234-123456789112')

    # Test with 16-byte UUID (Little Endian)
    string = '12917856-3412-3412-3412-341278563412'
    uuid = string_to_uuid(string, input_byteorder='little')
    assert uuid == UUID('12345678-1234-1234-1234-123456789112')


def test_uuid_to_string():
    """ Test to convert a uuid into a string. """
    # Test with 2-byte UUID (Little Endian)
    uuid = UUID('00001234-0000-1000-8000-00805f9b34fb')
    string = uuid_to_string(uuid)
    assert string == '3412'

    # Test with 2-byte UUID (Big Endian)
    uuid = UUID('00001234-0000-1000-8000-00805f9b34fb')
    string = uuid_to_string(uuid, output_byteorder='big')
    assert string == '1234'

    # Test with 16-byte UUID (Little Endian)
    uuid = UUID('12345678-1234-1234-1234-123456789112')
    string = uuid_to_string(uuid)
    assert string == '12917856341234123412341278563412'

    # Test with 16-byte UUID (Big Endian)
    uuid = UUID('12345678-1234-1234-1234-123456789112')
    string = uuid_to_string(uuid, output_byteorder='big')
    assert string == '12345678123412341234123456789112'
