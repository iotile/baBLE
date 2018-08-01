# Release Notes

All major changes in each released version of **baBLE** are listed here.

## 1.2.0
- Global
  - add possibility to set connection interval on connect (95da8a3)
  - add possibility to run bable bridge only on 1 controller (to prevent conflicts) (56bddd5)
  - add flow control to prevent controller buffer overflow (#20) (bcdea47)
  - add peripheral features (#19) (2275d5f)
  - add parameters to specify start handle and end handle in ProbeCharacteristics (ba1c431)
  - add option to not stop on SIGINT (ctrl-c) (8b99597)
  - add status code in DeviceDisconnected event to know why (6557e62)
  - add connection handle to list_connected_devices (#15) (ffbf542)
  - fix attribute_handle in NotificationReceived was a string (now set to uint16) (4a6b2b3)
 
- Python
  - add pytest plugin to a mocked BaBLEInterface for tests (50efc9a)
  - add methods to Characteristic and Service to test equality + use them as dict key (ab6c26d)
  - add possibility to restrict handlers to a controller id (2952f8a)
  - export Controller and Device + add possibility to create them manually (9ddc0c6)
  - set default timeout to None (no timeout) (5612f6f)
  - add on_connected and on_disconnected handlers (25b1d34)
  - split connect into connect, probe_services and probe_characteristics (1644584)
  - handle error if message is str in bable_exception (60ca9d5)
  - remove callbacks on disconnect (df9335d)
  - change enable/disable_notification to set_notification with Characteristic as param (4c4cae4)
  - return address type as string ('random' or 'public') (296510d)
  - handle unicode strings in Python2 (20c3a92)
  - add custom params to callback functions (cb44145)
  - add BaBLEException to __init__ to make it accessible (810ca75)
  - add on_notification_set callback to enable/disable_notification (5f15315)
  - handle error while probing services/characteristics during connection (f6d98b2)
  - write_without_response does not have timeout (147da61)
  - fix issue with endianness on uuid (688578d)
  - bytes from flatbuffers packets were decoded (raising errors if not ascii) (26225f9)
 
- Cpp
  - add TXPowerLevel in parse EIR (ignored) to prevent multiple useless logs (ba142ae)
  - return DeviceConnected event with error code if L2CAP failed (2baca99)
  - config handle was not got if last handle (cd0015f)
  - decrease verbosity of log in debug level (ef76aad)
  - rename bootstrap to registration (4395909)
  - add comments to tests (4c3a19a)
  - remove callback from timestamp callbacks to prevent useless expiration (2b9fec6)
  - set correct native_class in packets (27d9339)
  - handle UUID16 with service data in EIR + change log level to debug on unknown EIR type (a4113e9)
  - fix bug WriteWithoutResponsePeripheral sending back WritePeripheral response (e3b8dd0)
  - fix bug on ReadByTypeRequest characteristics configurations (92b50b7)
  - fix bugs with EmitNotification (78ad5f4)
  - fix bug in ProbeCharacteristics if start_handle/end_handle were given (c79ece5)

## 1.1.0

- Python
  - Add `active_scan` parameter to `start_scan` to let the user choose to make either a passive or an active scan.

## 1.0.2

- Improve READMEs
- Add tests (C++ and Python)
- Python
  - Fix bugs:
    - Prevent to start `bable_interface` multiple times
    - Cleanly stop tasks when stopping `bable_interface`
    - Add `sync` parameter to `bable_interface.stop()`
    - Correct exception raised when running a command on a stopped `bable_interface`
    - Multiple callbacks can be removed at once in `CommandsManager`
    - Replace `TimeoutError` exceptions by `RuntimeError` to be compatible with Python2
    - Error message is correctly extracted in `BaBLEException`
    - Models are stringified correctly (`__str__` calls `__repr__`)
    - Handle `uuid` and `native_class` empty in `Packet` models
  - Make `to_bytes` to have the same behaviour in Python2 and Python3
  - Allow to build python package without `VERSION` env variable to make it work in `tox` (if not set, use `0.0.0.dev0` as version)
  - Clean code (double quotes/single quotes)
- C++
  - Build `bable` library separated from the executable
  - Create a C++ wrapper around C socket library (called `Socket`) to make it easy to test
  - Add `--help` option
  - Fix bugs:
    - Wrong controller id was set in `GetControllersList` response
    - `Utils::string_to_number()` was not returning value (magically worked thanks to pointers stuff)
    
## 1.0.1

- Fix bugs:
  - Packets in queue in MGMT/HCI socket never sent
  - Old `remove_response_callback` function not removed in `list_controllers`
  - Replace `__str__` by `__repr__` in models/ to act like expected

## 1.0.0

- Initial release.
- Contains a Linux bridge and a Python interface to interact with Bluez and send
BLE commands.
