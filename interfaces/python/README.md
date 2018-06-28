# baBLE - Python interface

This is the cross-platform Python interface to communicate with the platform-dependent baBLE bridge, to use
native Bluetooth Low Energy in your projects.

## Usage

```python
import bable_interface  # Import the library
import time

bable = bable_interface.BaBLEInterface()  # Create a BaBLEInterface object to send commands
connections = {}

def on_notification_received(success, result, failure_reason):
    print("Notification received:", result, failure_reason)


def on_connected(success, result, failure_reason):
    print("Connected callback called", result, failure_reason)
    if not success:
        bable.cancel_connection()
    else:
        connections[result['address']] = result
        # Command can be forced to be asynchronous by setting `sync` to False (if command is sync by default)
        # It is often useful if you want to call a command into a callback function (because they must not be blocking)
        bable.enable_notification(result['connection_handle'], 0x000a, on_notification_received, sync=False)


def on_unexpected_disconnection(success, result, failure_reason):
    print("Device has been unexpectedly disconnected", result, failure_reason)


def on_device_found(success, result, failure_reason):
    print("Device found:", result, failure_reason)
    if result['address'] == "11:22:33:44:55:66":
        print("Connecting...")
        # You can precise timeout duration (meaning the time before an exception is raised if no response has been received)
        # by setting the `timeout` parameter with the value you want (in seconds)
        bable.connect(result['address'], "random", on_connected, on_unexpected_disconnection, timeout=5.0)


def on_disconnected(success, result, failure_reason):
    print("Disconnected callback called", result, failure_reason)


def on_error(status, message):
    print("Error received:", status, message)
    bable.stop()


# Start the bable interface (meaning starting the threads and the subprocess needed to make it work)
bable.start(on_error)

# Commands that could take a long time are asynchronous by default: result is sent by calling a callback function.
# If an error occures, callback function will be called with `success` parameter as False and failure_reason as the error
bable.start_scan(on_device_found)

time.sleep(10)  # We can sleep here without blocking the interface (because it is running in another thread)

try:
    print("Disconnecting...")
    # Command can be forced to be synchronous by setting `sync` to True (if command is async by default)
    bable.disconnect(connections['11:22:33:44:55:66'], sync=True)  # Will block until disconnected
    print("Disconnected")
except Exception as e:  # Synchronous commands raise exception on error
    print("Error while disconnecting:", e)

# When we have finished, we have to stop the bable interface to cleanly terminate the threads and the subprocess.
bable.stop()
print("End")
```
