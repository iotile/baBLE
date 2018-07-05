import bable_interface
from bable_interface.utils import none_cb
import pytest


@pytest.mark.timeout(10)
def test_start_stop(bridge_subprocess):
    bable = bable_interface.BaBLEInterface()
    assert bable.started is False

    with pytest.raises(RuntimeError):
        bable.start_scan(none_cb)

    bable.start()
    assert bable.started is True

    bable.start_scan(none_cb, sync=False)

    bable.stop()
    assert bable.started is False


@pytest.mark.timeout(10)
def test_restart(bridge_subprocess):
    bable = bable_interface.BaBLEInterface()

    bable.start()
    bable.stop()

    bridge_subprocess.reset()

    bable.start()
    assert bable.started is True
    bable.stop()
    assert bable.started is False


@pytest.mark.timeout(10)
def test_stop_not_started(bridge_subprocess):
    bable = bable_interface.BaBLEInterface()

    with pytest.raises(RuntimeError):
        bable.stop()


@pytest.mark.timeout(10)
def test_start_twice(bridge_subprocess):
    bable = bable_interface.BaBLEInterface()

    bable.start()

    with pytest.raises(RuntimeError):
        bable.start()
