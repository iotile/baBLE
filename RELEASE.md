# Release Notes

All major changes in each released version of **baBLE** are listed here.

## 1.0.1

- Fix bugs:
  - Packets in queue in MGMT/HCI socket never sent
  - Old `remove_response_callback` function not removed in `list_controllers`
  - Replace `__str__` by `__repr__` in models/ to act like expected

## 1.0.0

- Initial release.
- Contains a Linux bridge and a Python interface to interact with Bluez and send
BLE commands.
