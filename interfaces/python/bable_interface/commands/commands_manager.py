import functools
import logging
import threading


class CommandsManager(object):

    def __init__(self, subprocess):
        self.logger = logging.getLogger(__name__)

        self.callbacks = []
        self.callback_lock = threading.Lock()

        self.subprocess = subprocess

    # Executed into the ReceiverThread
    def handle(self, packet, add_task_fn):
        packet_uuid = packet.packet_uuid
        found = False

        with self.callback_lock:
            for uuid, callback in self.callbacks:
                if uuid.match(packet_uuid):
                    found = True
                    add_task_fn(callback(packet=packet))

        if not found:
            self.logger.info("Unexpected response received (uuid=%s)", packet_uuid)
            return False

        return True

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
            for index, (uuid, _) in enumerate(self.callbacks):
                for uuid_to_remove in packet_uuids:
                    if uuid.match(uuid_to_remove):
                        to_remove.append(index)

            for index in reversed(to_remove):
                del self.callbacks[index]

    def send_packet(self, packet):
        self.subprocess.stdin.write(packet.serialize())
