import threading
import subprocess
import time

from .BaBLE import Payload, Exit
from .flatbuffer import build_packet
from .working_thread import WorkingThread
from .receiving_thread import ReceivingThread
from .commands_manager import CommandsManager


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

    def start_scan(self, on_device_found, controller_id=0):
        calldone = threading.Event()

        def scan_started(fut):
            calldone.set()

        self.working_thread.add_task(
            self.commands_manager.start_scan(controller_id, on_device_found),
            scan_started
        )
        calldone.wait()

    def stop_scan(self, controller_id=0):
        calldone = threading.Event()

        def scan_stopped(fut):
            calldone.set()

        self.working_thread.add_task(
            self.commands_manager.stop_scan(controller_id),
            scan_stopped
        )
        calldone.wait()

    def connect(self, address, address_type, on_connected, on_disconnected, controller_id=0):
        self.working_thread.add_task(
            self.commands_manager.connect(controller_id, address, address_type, on_connected, on_disconnected)
        )

    def disconnect(self, connection_handle, on_disconnected, controller_id=0):
        self.working_thread.add_task(
            self.commands_manager.disconnect(controller_id, connection_handle, on_disconnected)
        )

    def cancel_connection(self, controller_id=0):
        self.working_thread.add_task(
            self.commands_manager.cancel_connection(controller_id)
        )

    def list_connected_devices(self, controller_id=0):
        calldone = threading.Event()
        result = [None]

        def list_received(fut):
            result[0] = fut.result()
            calldone.set()

        self.working_thread.add_task(
            self.commands_manager.list_connected_devices(controller_id),
            list_received
        )
        calldone.wait()
        return result[0]

    def list_controllers(self):
        calldone = threading.Event()
        result = [None]

        def list_received(fut):
            result[0] = fut.result()
            calldone.set()

        self.working_thread.add_task(
            self.commands_manager.list_controllers(),
            list_received
        )
        calldone.wait()
        return result[0]

    def read(self, connection_handle, attribute_handle, on_read_callback, controller_id=0):
        self.working_thread.add_task(
            self.commands_manager.read(controller_id, connection_handle, attribute_handle, on_read_callback)
        )

    def write(self, connection_handle, attribute_handle, value, on_written_callback, controller_id=0):
        self.working_thread.add_task(
            self.commands_manager.write(controller_id, connection_handle, attribute_handle, value, on_written_callback)
        )

    def write_without_response(self, connection_handle, attribute_handle, value, controller_id=0):
        self.working_thread.add_task(
            self.commands_manager.write_without_response(controller_id, connection_handle, attribute_handle, value)
        )
