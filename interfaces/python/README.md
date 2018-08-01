# baBLE - Python interface

This is the cross-platform Python interface to communicate with the platform-dependent baBLE bridge, to use
native Bluetooth Low Energy in your projects.

- [Usage](#usage)
- [Installation](#installation)
- ["bable" script](#"bable"-script)
- [Installing Support for IOTile Based Devices](#installing-support-for-iotile-based-devices)
- [How to use it without sudo](#how-to-use-it-without-sudo)


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
        char = bable_interface.Characteristic(uuid='1234', handle='0x000a', value_handle='0x000b')
        # Command can be forced to be asynchronous by setting `sync` to False (if command is sync by default)
        # It is often useful if you want to call a command into a callback function (because they must not be blocking)
        bable.set_notification(True, result['connection_handle'], char, on_notification_received=on_notification_received, sync=False)


def on_unexpected_disconnection(success, result, failure_reason):
    print("Device has been unexpectedly disconnected", result, failure_reason)


def on_device_found(success, result, failure_reason):
    print("Device found:", result, failure_reason)
    if result['address'] == "11:22:33:44:55:66":
        print("Connecting...")
        # You can precise timeout duration (meaning the time before an exception is raised if no response has been received)
        # by setting the `timeout` parameter with the value you want (in seconds)
        bable.connect(result['address'], result['address_type'], on_connected, on_unexpected_disconnection, timeout=5.0)


def on_disconnected(success, result, failure_reason):
    print("Disconnected callback called", result, failure_reason)


def on_error(status, message):
    print("Error received:", status, message)
    bable.stop(sync=False)


# Start the bable interface (meaning starting the threads and the subprocess needed to make it work)
# You can restrict baBLE to only one controller by setting the controller_id (if you want to use multiple controllers)
bable.start(on_error=on_error, controller_id=1)

# Commands that could take a long time are asynchronous by default: result is sent by calling a callback function.
# If an error occures, callback function will be called with `success` parameter as False and failure_reason as the error
bable.start_scan(on_device_found, timeout=1)

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

## Installation

Currently, only Linux is available (Windows and Mac coming soon...)

#### From Pypi

It is recommended to install it from Pypi to have a ready-to-go package with a pre-compiled bridge in it.

```bash
pip install bable-interface
```

This technique will work for for x86_64, i686 and armv7l architecture.

#### From source

You can also build it from source but you'll have to compile the bridge by yourself
(cf [platforms/](https://github.com/iotile/baBLE/tree/master/platforms) to see how to build your bridge from source).

```bash
$ git clone https://github.com/iotile/baBLE.git
$ cd baBLE/interfaces/python

# Here you can either copy your compiled bridge to the bin folder to add it to your package...
$ cp <path_to_your_bridge_exe> ./bable_interface/bin

# ... or you can add the folder where the bridge executable is to your PATH (and add use_path=True to BaBLEInterface.start())
$ export PATH=<path_to_your_bridge_exe_folder>:$PATH

$ python setup.py install
```

## "bable" script

After installing the Python bable interface, you'll have access to the `bable` command from your shell. It could be useful
for 2 things:
  - get the current bable version installed (`bable --version`)
  - set the capabilities of the bridge executable ([cf "How to use it without sudo" part](#how-to-use-it-without-sudo))
  (`bable --set-cap`)

## How to use it without sudo

By default, on Linux, you need to have root rights to communicate with the Bluetooth subkernel. But if you don't want to
execute your program as sudo, you can simply set the capabilities of the bridge executable. 
The simplest way to do so is simply to run:

```bash
$ bable --set-cap
```

This will simply run the following command, that you can obviously run by yourself, especially if you want to use a
different bable bridge:

```bash
$ sudo setcap cap_net_raw,cap_net_admin+eip <path_to_your_bridge_exe>
```
