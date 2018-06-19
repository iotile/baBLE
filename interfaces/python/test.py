import bable_interface
import time
import logging

log = logging.getLogger()

console = logging.StreamHandler()
console.setFormatter(logging.Formatter('%(asctime)s %(levelname).3s [%(name)s] %(message)s', '%y-%m-%d %H:%M:%S'))
log.addHandler(console)

log.setLevel(logging.DEBUG)

bable = bable_interface.BaBLEInterface()


def on_device_found(success, result, failure_reason):
    print("ON DEVICE FOUND", success, result, failure_reason)


def on_read(success, result, failure_reason):
    print("ON READ", success, result, failure_reason)


def on_write(success, result, failure_reason):
    print("ON WRITE", result, failure_reason)
    if success:
        bable.read(result["connection_handle"], 0x0003, on_read)


def on_notification_received(success, result, failure_reason):
    print("ON NOTIFICATION RECEIVED", success, result, failure_reason)


def on_connected(success, result, failure_reason):
    print("ON CONNECTED", result, failure_reason)
    if not success:
        bable.cancel_connection()
    else:
        # bable.enable_notification(0x0040, 0x000e, on_notification_received, sync=False)
        # print("NOTIFICATION ENABLED")
        bable.read(result["connection_handle"], 0x0003, on_read)
        # bable.write(result["connection_handle"], 0x0003, bytes("Cafe", encoding="utf-8"), on_write)
        # bable.write_without_response(result["connection_handle"], 0x0003, bytes("Arch", encoding="utf-8"))


def on_unexpected_disconnection(success, result, failure_reason):
    print("ON UNEXPECTED DISCONNECTION", success, result, failure_reason)


def on_disconnected(success, result, failure_reason):
    print("ON DISCONNECTED", success, result, failure_reason)


def on_error(status, message):
    print("ON ERROR", status, message)
    bable.stop()


bable.start(on_error=on_error)

# try:
#     bable.start_scan(on_device_found, timeout=1)
#     print("SCAN STARTED IN TEST.PY")
# except Exception as e:
#     print("EXCEPTION CAUGHT IN TEST.PY", e)
#
# time.sleep(0.2)
# bable.stop_scan(timeout=1)
# print("SCAN STOPPED IN TEST.PY")
# time.sleep(2)
# bable.stop()
# print("DONE")

try:
    # bable.connect("C4:F0:A5:E6:8A:01", "random", sync=True)
    bable.connect("C4:F0:A5:E6:8A:91", "random", on_connected, on_unexpected_disconnection, timeout=5.0)
    print("CONNECTED IN TEST.PY")
    time.sleep(10)
    # bable.read(0x0040, 0x0003, on_read)
    # time.sleep(5)
    bable.disconnect(0x0040, on_disconnected, sync=True)
    print("DISCONNECTED IN TEST.PY")

    # devices = bable.list_controllers()
    # print(devices)
except Exception as e:
    print("Exception caught in test.py={}".format(e))

time.sleep(2)
bable.stop()
print("DONE")
