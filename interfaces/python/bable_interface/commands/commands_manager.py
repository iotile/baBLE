import functools
import threading


class CommandsManager(object):

    def __init__(self, subprocess):
        self.callbacks = []
        self.callback_lock = threading.Lock()
        self.subprocess = subprocess

    # Executed into the ReceiverThread
    def handle(self, packet, add_task_fn):
        packet_uuid = packet.packet_uuid
        found = False

        with self.callback_lock:
            for index, (uuid, callback) in enumerate(self.callbacks):
                if uuid.match(packet_uuid):
                    found = True
                    add_task_fn(callback(packet=packet))

        if not found:
            print("Unexpected response received (uuid={})".format(packet_uuid))  # TODO: use logger instead
            return

    # Executed into the WorkingThread
    def register_callback(self, packet_uuid, callback, params=None, replace=False):
        if replace:
            self.remove_callback(packet_uuid)

        if params is None:
            params = {}

        callback_with_args = functools.partial(callback, **params)

        with self.callback_lock:
            self.callbacks.append((packet_uuid, callback_with_args))

    # Executed into the WorkingThread
    def remove_callback(self, *packet_uuids):
        to_remove = []

        with self.callback_lock:
            for index, (uuid, callback) in enumerate(self.callbacks):
                if uuid in packet_uuids:
                    to_remove.append(index)

            for index in to_remove:
                del self.callbacks[index]

    def send_packet(self, packet):
        self.subprocess.stdin.write(packet.serialize())
