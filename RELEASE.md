# Release Notes

All major changes in each released version of **baBLE** are listed here.

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
