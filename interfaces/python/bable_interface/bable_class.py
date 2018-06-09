import threading
import subprocess
import time

from .BaBLE import Payload, Exit
from .flatbuffer import build_packet
from .working_thread import WorkingThread
from .receiving_thread import ReceivingThread
from .commands_manager import CommandsManager
from .utils import none_cb


class BaBLEInterface(object):

    def __init__(self):
        self.working_ready_event = threading.Event()
        self.working_thread = WorkingThread(self.working_ready_event)

        self.receiving_thread = None
        self.stop_receiving_event = threading.Event()

        self.subprocess = None
        self.subprocess_ready_event = threading.Event()

        self.commands_manager = None

    def start(self):
        self.working_thread.start()
        self.working_ready_event.wait()
        self.subprocess = subprocess.Popen(["../../platforms/linux/build/debug/baBLE_linux", "--logging", "info"],
                                           stdout=subprocess.PIPE, stdin=subprocess.PIPE,
                                           bufsize=0,
                                           universal_newlines=False)

        self.commands_manager = CommandsManager(self.subprocess)

        self.receiving_thread = ReceivingThread(self.stop_receiving_event, self.on_receive, self.subprocess.stdout)
        self.receiving_thread.setDaemon(True)
        self.receiving_thread.start()

        self.subprocess_ready_event.wait()

    def stop(self):
        self.working_thread.stop()

        self.subprocess.stdin.write(build_packet(Exit))

        while self.subprocess.poll() is None:
            print("Waiting for subprocess to stop")
            time.sleep(0.1)

        self.receiving_thread.join(5)
        if self.receiving_thread.isAlive():
            print("Timeout while waiting for receiving thread to stop...")

    def on_receive(self, packet):
        if packet.PayloadType() == Payload.Payload.Ready:
            self.subprocess_ready_event.set()
        else:
            self.commands_manager.handle(packet, self.working_thread.add_task)

    def on_done(self, fut, event, return_value):
        if fut.exception() is not None:
            return_value.update({
                "success": False,
                "result": None,
                "failure_reason": fut.exception()
            })
        else:
            return_value.update({
                "success": True,
                "result": fut.result(),
                "failure_reason": None
            })

        event.set()

    def run_command(self, command, sync):
        calldone = threading.Event()
        result = {}

        self.working_thread.add_task(task=command, callback=self.on_done, event=calldone, return_value=result)

        if sync:
            calldone.wait()
            if result.get("success", False):
                return result["result"]
            else:
                raise RuntimeError(result.get("failure_reason", "No result returned"))

    def start_scan(self, on_device_found, controller_id=0, sync=True):
        return self.run_command(command=self.commands_manager.start_scan(controller_id, on_device_found), sync=sync)

    def stop_scan(self, controller_id=0, sync=True):
        return self.run_command(command=self.commands_manager.stop_scan(controller_id), sync=sync)

    def connect(self, address, address_type, on_connected=none_cb, on_disconnected=none_cb, controller_id=0, sync=False):
        return self.run_command(
            command=self.commands_manager.connect(controller_id, address, address_type, on_connected, on_disconnected),
            sync=sync
        )

    def disconnect(self, connection_handle, on_disconnected=none_cb, controller_id=0, sync=False):
        return self.run_command(
            command=self.commands_manager.disconnect(controller_id, connection_handle, on_disconnected),
            sync=sync
        )

    def cancel_connection(self, controller_id=0, sync=False):
        return self.run_command(command=self.commands_manager.cancel_connection(controller_id), sync=sync)

    def list_connected_devices(self, controller_id=0):
        return self.run_command(command=self.commands_manager.list_connected_devices(controller_id), sync=True)

    def list_controllers(self):
        return self.run_command(command=self.commands_manager.list_controllers(), sync=True)

    def read(self, connection_handle, attribute_handle, on_read=none_cb, controller_id=0, sync=False):
        return self.run_command(
            command=self.commands_manager.read(controller_id, connection_handle, attribute_handle, on_read),
            sync=sync
        )

    def write(self, connection_handle, attribute_handle, value, on_written=none_cb, controller_id=0, sync=False):
        return self.run_command(
            command=self.commands_manager.write(controller_id, connection_handle, attribute_handle, value, on_written),
            sync=sync
        )

    def write_without_response(self, connection_handle, attribute_handle, value, controller_id=0, sync=False):
        return self.run_command(
            command=self.commands_manager.write_without_response(controller_id, connection_handle, attribute_handle, value),
            sync=sync
        )

    def enable_notification(self, connection_handle, attribute_handle, on_notification_received=none_cb, controller_id=0, sync=True):
        return self.run_command(
            command=self.commands_manager.set_notification(True, controller_id, connection_handle, attribute_handle, on_notification_received),
            sync=sync
        )

    def disable_notification(self, connection_handle, attribute_handle, controller_id=0, sync=True):
        return self.run_command(
            command=self.commands_manager.set_notification(False, controller_id, connection_handle, attribute_handle),
            sync=sync
        )
