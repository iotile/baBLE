import pytest
from bable_interface.models import Packet
from bable_interface.BaBLE import Payload, ReadCentral


def test_build():
    """ Test building a packet. """
    packet = Packet.build(
        payload_module=ReadCentral,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )

    assert isinstance(packet, Packet)
    assert packet.controller_id == 0
    assert packet.payload_type == Payload.Payload.ReadCentral
    assert packet.get('connection_handle') == 0x0040
    assert packet.get('attribute_handle') == 0x0003

    # Try to get a parameter that does not exist in the protocol
    with pytest.raises(KeyError):
        Packet.build(
            payload_module=ReadCentral,
            does_not_exist="test"
        )


def test_extract():
    """ Test extract packet information from raw bytes. """
    # These raw bytes are from the ReadCentral packet built in test_build()
    raw_payload = bytearray(b'\x10\x00\x00\x00\x0c\x00\x10\x00\x0c\x00\x09\x00\x04\x00\x0a\x00\x0c\x00\x00\x00\x1c\x00'
                            b'\x00\x00\x00\x0b\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x08\x00'
                            b'\x06\x00\x04\x00\x08\x00\x00\x00\x03\x00\x40\x00')
    packet = Packet.extract(raw_payload)

    assert isinstance(packet, Packet)
    assert packet.controller_id == 0
    assert packet.payload_type == Payload.Payload.ReadCentral
    assert packet.get('connection_handle') == 0x0040
    assert packet.get('attribute_handle') == 0x0003

    # Test with a wrong payload bytes (random)
    raw_payload2 = bytearray(b'\x10\x00\x00\x00\x0c\x00')
    with pytest.raises(Exception):
        Packet.extract(raw_payload2)


def test_serialize():
    """ Test to serialize a packet (getting raw bytes from packet object). """
    packet = Packet(
        payload_type=Payload.Payload.ReadCentral,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )

    assert packet.serialize() is not None


def test_get():
    """ Test to get packet parameters. """
    packet = Packet(
        payload_type=Payload.Payload.ReadCentral,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )

    # Test to get a single parameter
    assert packet.get('connection_handle') == 0x0040

    # Test to get a dictionary with multiple parameters
    expected_dict = {'connection_handle': 0x0040, 'attribute_handle': 0x0003}
    assert packet.get_dict(['connection_handle', 'attribute_handle']) == expected_dict

    # Test to get a dictionary with format function
    expected_dict['attribute_handle'] = 'test'
    assert packet.get_dict(['connection_handle', ('attribute_handle', lambda value: 'test')]) == expected_dict
