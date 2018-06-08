import inspect
import sys
from .flatbuffer import build_packet, get_packet_uuid

if sys.version_info < (3, 4):
    import bable_interface.commands_py2 as commands_module
else:
    import bable_interface.commands_py3 as commands_module


class CommandsManager(object):

    def __init__(self, subprocess):
        self.responses_callbacks = {}
        self.events_callbacks = {}

        self.subprocess = subprocess

    # TODO: create a PacketUuid object and create a list of tuples
    def _find_event_callback(self, key):
        for packet_uuid, callback_with_params in self.events_callbacks.items():
            if key[0] == packet_uuid[0] \
               and key[1] == packet_uuid[1] \
               and (packet_uuid[2] is None or (key[2] is not None and key[2].lower() == packet_uuid[2].lower())) \
               and (packet_uuid[3] is None or key[3] == packet_uuid[3]) \
               and (packet_uuid[4] is None or key[4] == packet_uuid[4]):
                return callback_with_params

        return None

    def handle(self, packet, add_task_fn):
        uuid = packet.Uuid()

        if uuid:
            uuid = uuid.decode()
            if uuid in self.responses_callbacks:
                callback_fn, kwargs = self.responses_callbacks[uuid]
                del self.responses_callbacks[uuid]
                add_task_fn(callback_fn(packet, **kwargs))
            else:
                print("Unexpected response received (uuid={})".format(uuid))
        else:
            uuid = get_packet_uuid(packet)
            # print("UUID COMPUTED", uuid)
            event_callback = self._find_event_callback(uuid)
            if event_callback is not None:
                callback_fn, kwargs = event_callback
                add_task_fn(callback_fn(packet, **kwargs))
            else:
                print("Unexpected event received (uuid={})".format(uuid))

    def register_response_callback(self, uuid, callback_fn, **cb_kwargs):
        if uuid in self.responses_callbacks:
            raise KeyError("UUID already registered in responses callbacks (uuid={})".format(uuid))

        self.responses_callbacks[uuid] = (callback_fn, cb_kwargs)

    def register_event_callback(self, payload_type, controller_id, callback_fn,
                                connection_handle=None, attribute_handle=None, address=None,
                                replace=False, **cb_kwargs):
        key = (payload_type, controller_id, address, connection_handle, attribute_handle)

        if key in self.events_callbacks:
            if not replace:
                raise KeyError("Event key already registered in events callbacks (key={})".format(key))

        self.events_callbacks[key] = (callback_fn, cb_kwargs)

    def remove_event_callback(self, payload_type, controller_id, connection_handle=None, attribute_handle=None,
                              address=None):
        key = (payload_type, controller_id, address, connection_handle, attribute_handle)

        if key in self.events_callbacks:
            del self.events_callbacks[key]

    def remove_response_callback(self, uuid):
        if uuid in self.responses_callbacks:
            del self.responses_callbacks[uuid]

    def send_packet(self, payload_module, *args, **kwargs):
        self.subprocess.stdin.write(build_packet(payload_module, *args, **kwargs))


commands = inspect.getmembers(commands_module, inspect.isfunction)
for command_name, command_fn in commands:
    setattr(CommandsManager, command_name, command_fn)
