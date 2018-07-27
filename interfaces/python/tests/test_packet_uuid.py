from bable_interface.models import PacketUuid
from bable_interface.BaBLE.Payload import Payload


def test_match():
    """ Test cases where packet uuids should match. """
    packet_uuid1 = PacketUuid(
        payload_type=Payload.ReadCentral,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )

    # Packet uuids are equal
    assert packet_uuid1.match(packet_uuid1)

    # Test with different packet that should still match (same required information)
    packet_uuid2 = PacketUuid(
        payload_type=Payload.ReadCentral,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003,
        address="11:22:33:44:55:66"
    )
    assert packet_uuid1.match(packet_uuid2)

    # Test with uuids
    packet_uuid1.set(uuid="test")
    packet_uuid3 = PacketUuid(uuid="test")
    assert packet_uuid1.match(packet_uuid3)

    # Test with a generic packet uuid (only payload_type)
    packet_uuid4 = PacketUuid(
        payload_type=Payload.ReadCentral
    )

    packet_uuid5 = PacketUuid(
        payload_type=Payload.WriteCentral
    )

    assert packet_uuid4.match(packet_uuid1)
    assert packet_uuid4.match(packet_uuid2)
    assert not packet_uuid4.match(packet_uuid5)

    # Test with match_connection_only flag
    packet_uuid6 = PacketUuid(
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0000
    )

    packet_uuid7 = PacketUuid(
        payload_type=Payload.ReadCentral,
        controller_id=0,
        connection_handle=0x0041,
        attribute_handle=0x0000
    )

    assert not packet_uuid6.match(packet_uuid1)
    assert packet_uuid6.match(packet_uuid1, match_connection_only=True)
    assert packet_uuid6.match(packet_uuid2, match_connection_only=True)
    assert not packet_uuid6.match(packet_uuid7, match_connection_only=True)


def test_not_match():
    """ Test cases where packet uuids should not match. """
    packet_uuid1 = PacketUuid(
        payload_type=Payload.ReadCentral,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )

    # Payload types are different
    packet_uuid2 = PacketUuid(
        payload_type=Payload.WriteCentral,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )
    assert not packet_uuid1.match(packet_uuid2)

    # Controller ids are different
    packet_uuid3 = PacketUuid(
        payload_type=Payload.ReadCentral,
        controller_id=1,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )
    assert not packet_uuid1.match(packet_uuid3)

    # Connection handles are different
    packet_uuid4 = PacketUuid(
        payload_type=Payload.ReadCentral,
        controller_id=0,
        connection_handle=0x0041,
        attribute_handle=0x0003
    )
    assert not packet_uuid1.match(packet_uuid4)

    # Attribute handles are different
    packet_uuid5 = PacketUuid(
        payload_type=Payload.ReadCentral,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0001
    )
    assert not packet_uuid1.match(packet_uuid5)

    # Uuids are different
    packet_uuid6 = PacketUuid(
        payload_type=Payload.ReadCentral,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003,
        uuid="test"
    )
    assert not packet_uuid6.match(packet_uuid1)
