import pytest
from bable_interface.utils import to_bytes, camel_to_snake, snake_to_camel


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
