from bable_interface.commands.commands_manager import CommandsManager
from bable_interface.models import Packet, PacketUuid
from bable_interface.BaBLE import StartScan, Payload


def test_register_callback():
    """ Test to register a callback in the CommandsManager. """
    def callback():
        pass

    packet_uuid = PacketUuid()

    command_manager = CommandsManager(None)
    assert len(command_manager.callbacks) == 0

    command_manager.register_callback(packet_uuid=packet_uuid, callback=callback)
    assert len(command_manager.callbacks) == 1

    # Multiple callbacks with the same packet_uuid can be added
    command_manager.register_callback(packet_uuid=packet_uuid, callback=callback)
    assert len(command_manager.callbacks) == 2

    # If replace=True, all the callbacks with the same packet_uuid are removed before adding this one
    command_manager.register_callback(packet_uuid=packet_uuid, callback=callback, replace=True)
    assert len(command_manager.callbacks) == 1


def test_remove_callback():
    """ Test to remove a callback in the CommandsManager. """
    def callback():
        pass

    packet_uuid = PacketUuid()

    command_manager = CommandsManager(None)

    # Try to remove a callback that does not exist
    command_manager.remove_callback(packet_uuid)
    assert len(command_manager.callbacks) == 0

    command_manager.register_callback(packet_uuid=packet_uuid, callback=callback)
    assert len(command_manager.callbacks) == 1

    command_manager.remove_callback(packet_uuid)
    assert len(command_manager.callbacks) == 0


def test_handle(bridge_subprocess):
    """ Test if a packet is correctly handled if it is expected in CommandsManager. """
    def on_response_received(packet, test):
        assert packet.payload_type == Payload.Payload.StartScan
        assert test is True

    command_manager = CommandsManager(bridge_subprocess)

    # Create a request packet and register the response callback
    request_packet = Packet.build(StartScan, controller_id=0)
    command_manager.register_callback(
        request_packet.packet_uuid,
        callback=on_response_received,
        params={'test': True}
    )

    # If wrong response packet is received, should not handle it
    wrong_response_packet = Packet.build(StartScan, controller_id=0)
    assert command_manager.handle(wrong_response_packet, lambda cb: None) is False

    # If expected response packet received, should be handled
    good_response_packet = Packet.build(StartScan, controller_id=0)
    good_response_packet.packet_uuid.set(uuid=request_packet.packet_uuid.uuid)
    assert command_manager.handle(good_response_packet, lambda cb: None) is True


def test_send_packet(bridge_subprocess):
    """ Test sending a packet from CommandsManager. """
    command_manager = CommandsManager(bridge_subprocess)
    packet = Packet.build(StartScan, controller_id=0)

    command_manager.send_packet(packet)

    assert len(bridge_subprocess.stdin.input_buffer) == 1

    sent_packet = bridge_subprocess.stdin.input_buffer[0]
    assert sent_packet.payload_type == Payload.Payload.StartScan
    assert sent_packet.controller_id == 0
