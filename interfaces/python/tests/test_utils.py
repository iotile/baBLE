import pytest
from bable_interface.utils import to_bytes, camel_to_snake, snake_to_camel


def test_to_bytes():
    assert to_bytes(18062015, 4, byteorder='little') == b'\xbf\x9a\x13\x01'
    assert to_bytes(18062015, 4, byteorder='big') == b'\x01\x13\x9a\xbf'

    with pytest.raises(OverflowError):
        to_bytes(18062015, 2, byteorder='little')

    with pytest.raises(OverflowError):
        to_bytes(-18062015, 4, byteorder='little')

    with pytest.raises(AttributeError):
        to_bytes("18062015", 4, byteorder='little')

    with pytest.raises(AttributeError):
        to_bytes(18062015.5, 4, byteorder='little')


def test_camel_to_snake():
    assert camel_to_snake("CamelCase") == "camel_case"

    assert camel_to_snake("CamelHTTPCase") == "camel_http_case"

    assert camel_to_snake("camel_case") == "camel_case"


def test_snake_to_camel():
    assert snake_to_camel("snake_case") == "SnakeCase"

    assert snake_to_camel("snake_HTTP_case") == "SnakeHTTPCase"

    assert snake_to_camel("SnakeCase") == "SnakeCase"
