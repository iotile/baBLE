import logging
import os
import subprocess
import threading
import time
from .BaBLE import Payload, Exit
from .threads import WorkingThread, ReceivingThread
from .commands import CommandsManager
from .utils import none_cb
from .models import Packet


class BaBLEInterface(object):

    def __init__(self):
        self.logger = logging.getLogger(__name__)

        self.working_ready_event = threading.Event()
        self.working_thread = WorkingThread(self.working_ready_event)

        self.receiving_thread = None
        self.stop_receiving_event = threading.Event()

        self.subprocess = None
        self.subprocess_ready_event = threading.Event()

        self.commands_manager = None
        self.started = False

        self.on_error = None

    def start(self, on_error=None):
        self.on_error = on_error

        self.working_thread.start()
        self.working_ready_event.wait()

        logging_lvl = logging.getLevelName(self.logger.getEffectiveLevel())
        self.subprocess = subprocess.Popen(
            [os.path.join(os.path.dirname(__file__), "bin", "baBLE_linux"), "--logging", logging_lvl],
            stdout=subprocess.PIPE, stdin=subprocess.PIPE,
            bufsize=0,
            universal_newlines=False
        )

        self.commands_manager = CommandsManager(self.subprocess)

        self.receiving_thread = ReceivingThread(self.stop_receiving_event, self.on_receive, self.subprocess.stdout)
        self.receiving_thread.setDaemon(True)
        self.receiving_thread.start()

        self.subprocess_ready_event.wait()
        self.started = True

    def stop(self):
        if not self.started:
            raise RuntimeError("BaBLEInterface not running")
        self.started = False

        self.working_thread.stop()

        self.commands_manager.send_packet(Packet.build(Exit))

        while self.subprocess.poll() is None:
            self.logger.info("Waiting for subprocess to stop")
            time.sleep(0.1)

        self.receiving_thread.join(5)
        if self.receiving_thread.isAlive():
            self.logger.warning("Timeout while waiting for receiving thread to stop...")

    def on_receive(self, packet):
        if packet.payload_type == Payload.Payload.Ready:
            self.subprocess_ready_event.set()
            return

        if packet.payload_type == Payload.Payload.BaBLEError:
            if not packet.packet_uuid.uuid and self.on_error is not None:
                self.working_thread.add_task(task=self.commands_manager.handle_error(packet, self.on_error))
                return

        self.commands_manager.handle(packet, self.working_thread.add_task)

    def _run_command(self, command, sync):
        if not self.started:
            raise RuntimeError("BaBLEInterface not running")

        def on_done(fut, event, return_value):
            try:
                return_value.update({
                    "success": True,
                    "result": fut.result(),
                    "failure_reason": None
                })
            except Exception as e:
                return_value.update({
                    "success": False,
                    "result": None,
                    "failure_reason": e
                })

            event.set()

        calldone = threading.Event()
        result = {}

        self.working_thread.add_task(task=command, callback=on_done, event=calldone, return_value=result)

        if sync:
            calldone.wait()
            if result.get("success", False):
                return result.get("result")
            else:
                raise result.get("failure_reason", RuntimeError("Command failed without given failure reason"))

    #### Commands ####

    def start_scan(self, on_device_found, controller_id=0, on_scan_started=none_cb, sync=True, timeout=15.0):
        return self._run_command(
            command=self.commands_manager.start_scan(controller_id, on_device_found, on_scan_started, timeout),
            sync=sync
        )

    def stop_scan(self, controller_id=0, on_scan_stopped=none_cb, sync=True, timeout=15.0):
        return self._run_command(
            command=self.commands_manager.stop_scan(controller_id, on_scan_stopped, timeout),
            sync=sync
        )

    def connect(self, address, address_type, on_connected=none_cb, on_disconnected=none_cb, controller_id=0, sync=False,
                timeout=15.0):
        return self._run_command(
            command=self.commands_manager.connect(controller_id, address, address_type, on_connected, on_disconnected,
                                                  timeout),
            sync=sync
        )

    def disconnect(self, connection_handle, on_disconnected=none_cb, controller_id=0, sync=False, timeout=15.0):
        return self._run_command(
            command=self.commands_manager.disconnect(controller_id, connection_handle, on_disconnected, timeout),
            sync=sync
        )

    def cancel_connection(self, controller_id=0, on_connection_cancelled=none_cb, sync=False, timeout=15.0):
        return self._run_command(
            command=self.commands_manager.cancel_connection(controller_id, on_connection_cancelled, timeout),
            sync=sync
        )

    def list_connected_devices(self, controller_id=0, timeout=15.0):
        return self._run_command(
            command=self.commands_manager.list_connected_devices(controller_id, timeout),
            sync=True
        )

    def list_controllers(self, timeout=15.0):
        return self._run_command(
            command=self.commands_manager.list_controllers(timeout),
            sync=True
        )

    def read(self, connection_handle, attribute_handle, on_read=none_cb, controller_id=0, sync=False, timeout=15.0):
        return self._run_command(
            command=self.commands_manager.read(controller_id, connection_handle, attribute_handle, on_read, timeout),
            sync=sync
        )

    def write(self, connection_handle, attribute_handle, value, on_written=none_cb, controller_id=0, sync=False,
              timeout=15.0):
        return self._run_command(
            command=self.commands_manager.write(controller_id, connection_handle, attribute_handle, value, on_written,
                                                timeout),
            sync=sync
        )

    def write_without_response(self, connection_handle, attribute_handle, value, controller_id=0, sync=False,
                               timeout=15.0):
        return self._run_command(
            command=self.commands_manager.write_without_response(controller_id, connection_handle, attribute_handle,
                                                                 value, timeout),
            sync=sync
        )

    def enable_notification(self, connection_handle, attribute_handle, on_notification_received=none_cb,
                            controller_id=0, sync=True, timeout=15.0):
        return self._run_command(
            command=self.commands_manager.set_notification(True, controller_id, connection_handle, attribute_handle,
                                                           on_notification_received, timeout),
            sync=sync
        )

    def disable_notification(self, connection_handle, attribute_handle, controller_id=0, sync=True, timeout=15.0):
        return self._run_command(
            command=self.commands_manager.set_notification(False, controller_id, connection_handle, attribute_handle,
                                                           timeout),
            sync=sync
        )
