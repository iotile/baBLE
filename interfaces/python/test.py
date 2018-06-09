import bable_interface
import time


bable = bable_interface.BaBLEInterface()


def on_device_found(success, result, failure_reason):
    print("ON DEVICE FOUND", result)


def on_read(success, result, failure_reason):
    print("ON READ", result)


def on_write(success, result, failure_reason):
    print("ON WRITE", result)
    if success:
        bable.read(result["connection_handle"], 0x0003, on_read)


def on_notification_received(success, result, failure_reason):
    print("ON NOTIFICATION RECEIVED", result)


def on_connected(success, result, failure_reason):
    print("ON CONNECTED", result)
    if not success:
        bable.cancel_connection()
        print(failure_reason)
    else:
        bable.enable_notification(0x0040, 0x000e, on_notification_received, sync=False)
        print("NOTIFICATION ENABLED")
        # bable.read(result["connection_handle"], 0x000e, on_read)
        # bable.write(result["connection_handle"], 0x0003, bytes("Cafe", encoding="utf-8"), on_write)
        # bable.write_without_response(result["connection_handle"], 0x0003, bytes("Arch", encoding="utf-8"))


def on_unexpected_disconnection(success, result, failure_reason):
    print("ON UNEXPECTED DISCONNECTION", result)


def on_disconnected(success, result, failure_reason):
    print("ON DISCONNECTED", result)


bable.start()

# try:
#     bable.start_scan(on_device_found )
#     print("SCAN STARTED IN TEST.PY")
# except Exception as e:
#     print("EXCEPTION CAUGHT IN TEST.PY", e)
#
# time.sleep(2)
# bable.stop_scan()
# print("SCAN STOPPED IN TEST.PY")

try:
    # bable.connect("C4:F0:A5:E6:8A:01", "random", sync=True)
    bable.connect("C4:F0:A5:E6:8A:91", "random", on_connected, on_unexpected_disconnection)
    print("CONNECTED IN TEST.PY")
    time.sleep(10)
    # bable.read(0x0040, 0x0003, on_read)
    # time.sleep(5)
    bable.disconnect(0x0040, on_disconnected)
    print("DISCONNECTED IN TEST.PY")
except Exception as e:
    print("Exception caught in test.py={}".format(e))

# devices = bable.list_controllers()
# print(devices)

time.sleep(2)
bable.stop()
print("DONE")
