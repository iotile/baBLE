from bable_interface.models import PacketUuid
from bable_interface.BaBLE.Payload import Payload


def test_match():
    packet_uuid1 = PacketUuid(
        payload_type=Payload.Read,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )

    assert packet_uuid1.match(packet_uuid1)

    packet_uuid2 = PacketUuid(
        payload_type=Payload.Read,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003,
        address="11:22:33:44:55:66"
    )

    assert packet_uuid1.match(packet_uuid2)

    packet_uuid1.set(uuid="test")
    packet_uuid3 = PacketUuid(uuid="test")

    assert packet_uuid1.match(packet_uuid3)


def test_not_match():
    packet_uuid1 = PacketUuid(
        payload_type=Payload.Read,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )

    packet_uuid2 = PacketUuid(
        payload_type=Payload.Write,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )
    assert not packet_uuid1.match(packet_uuid2)

    packet_uuid3 = PacketUuid(
        payload_type=Payload.Read,
        controller_id=1,
        connection_handle=0x0040,
        attribute_handle=0x0003
    )
    assert not packet_uuid1.match(packet_uuid3)

    packet_uuid4 = PacketUuid(
        payload_type=Payload.Read,
        controller_id=0,
        connection_handle=0x0041,
        attribute_handle=0x0003
    )
    assert not packet_uuid1.match(packet_uuid4)

    packet_uuid5 = PacketUuid(
        payload_type=Payload.Read,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0001
    )
    assert not packet_uuid1.match(packet_uuid5)

    packet_uuid6 = PacketUuid(
        payload_type=Payload.Read,
        controller_id=0,
        connection_handle=0x0040,
        attribute_handle=0x0003,
        uuid="test"
    )
    assert not packet_uuid6.match(packet_uuid1)
