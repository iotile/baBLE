import pytest
from bable_interface.models import Packet
from bable_interface.BaBLE import Payload, Read


def test_build():
    packet = Packet.build(
        payload_module=Read,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )

    assert isinstance(packet, Packet)
    assert packet.controller_id == 0
    assert packet.payload_type == Payload.Payload.Read
    assert packet.get('connection_handle') == 0x0040
    assert packet.get('attribute_handle') == 0x0003

    with pytest.raises(KeyError):
        Packet.build(
            payload_module=Read,
            does_not_exist="test"
        )


def test_extract():
    # These raw bytes are from the Read packet built in test_build()
    raw_payload = bytearray(b'\x10\x00\x00\x00\x0c\x00\x10\x00\x0c\x00\x09\x00\x04\x00\x0a\x00\x0c\x00\x00\x00\x40\x00'
                            b'\x00\x00\x00\x0a\x00\x00\x04\x00\x00\x00\x24\x00\x00\x00\x30\x61\x65\x37\x31\x39\x61\x30'
                            b'\x2d\x65\x61\x64\x39\x2d\x34\x32\x31\x34\x2d\x38\x30\x64\x31\x2d\x63\x39\x61\x61\x36\x65'
                            b'\x61\x34\x32\x38\x37\x30\x00\x00\x00\x00\x08\x00\x08\x00\x06\x00\x04\x00\x08\x00\x00\x00'
                            b'\x03\x00\x40\x00')
    packet = Packet.extract(raw_payload)

    assert isinstance(packet, Packet)
    assert packet.controller_id == 0
    assert packet.payload_type == Payload.Payload.Read
    assert packet.get('connection_handle') == 0x0040
    assert packet.get('attribute_handle') == 0x0003

    # Wrong payload bytes
    raw_payload2 = bytearray(b'\x10\x00\x00\x00\x0c\x00')

    with pytest.raises(Exception):
        Packet.extract(raw_payload2)


def test_serialize():
    packet = Packet(
        payload_type=Payload.Payload.Read,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )

    assert packet.serialize() is not None


def test_get():
    packet = Packet(
        payload_type=Payload.Payload.Read,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )

    assert packet.get('connection_handle') == 0x0040

    expected_dict = {'connection_handle': 0x0040, 'attribute_handle': 0x0003}
    assert packet.get_dict(['connection_handle', 'attribute_handle']) == expected_dict

    expected_dict['attribute_handle'] = 'test'
    assert packet.get_dict(['connection_handle', ('attribute_handle', lambda value: 'test')]) == expected_dict
