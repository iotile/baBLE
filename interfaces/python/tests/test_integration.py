import pytest
import threading
import bable_interface
from bable_interface.models import Packet
from bable_interface.BaBLE import DeviceFound, StartScan, Payload


@pytest.mark.timeout(20)
@pytest.mark.parametrize('mock_uuid', ['test'], indirect=True)
def test_integration(bridge_subprocess, mock_uuid):
    device_found_event = threading.Event()
    error_received_event = threading.Event()
    bable = bable_interface.BaBLEInterface()

    def on_device_found(success, result, failure_reason):
        assert success is True
        device_found_event.set()

    def on_error(status, message):
        assert status[0] == 'Success'
        assert message == 'Test'
        error_received_event.set()

    response_packet = Packet.build(
        payload_module=StartScan,
        controller_id=0
    )
    response_packet.packet_uuid.set(uuid=mock_uuid)

    event_packet = Packet.build(
        payload_module=DeviceFound,
        controller_id=0
    )

    bable.start(on_error=on_error)

    with pytest.raises(RuntimeError):
        bable.start_scan(on_device_found=on_device_found, timeout=0.5, sync=True)

    def on_start_scan(*packets):
        def send_response():
            for packet in packets:
                bridge_subprocess.simulate_data(packet.serialize())
        return send_response

    bridge_subprocess.on_receive(Payload.Payload.StartScan, on_start_scan(response_packet, event_packet))
    result = bable.start_scan(on_device_found=on_device_found, timeout=5.0, sync=False)
    assert result is None
    assert device_found_event.wait(timeout=5.0)

    result = bable.start_scan(on_device_found=on_device_found, timeout=5.0, sync=True)
    assert result is True

    response_packet.controller_id = 1
    event_packet.controller_id = 1
    device_found_event.clear()
    bridge_subprocess.on_receive(Payload.Payload.StartScan, on_start_scan(response_packet, event_packet))
    result = bable.start_scan(on_device_found=on_device_found, controller_id=1, timeout=5.0, sync=True)
    assert result is True
    assert device_found_event.wait(timeout=5.0)

    error_packet = Packet(
        payload_type=Payload.Payload.BaBLEError,
        controller_id=0,
        message="Test"
    )
    bridge_subprocess.simulate_data(error_packet.serialize())
    assert error_received_event.wait(timeout=5.0)

    bable.stop()
