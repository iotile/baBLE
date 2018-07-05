import threading
from bable_interface.models import Packet
from bable_interface.flatbuffers_functions import build_packet
from bable_interface.BaBLE.Payload import Payload


class MockStdIO(object):

    def __init__(self, process):
        self.process = process
        self.callbacks = {}

        self.lock = threading.Lock()

        self.data_available_event = threading.Event()
        self.index_read = 0
        self.output_buffer = bytearray()
        self.input_buffer = []

    def send(self, data):
        with self.lock:
            self.output_buffer += data
            self.data_available_event.set()

    def clear(self):
        with self.lock:
            self.data_available_event.clear()
            self.index_read = 0
            self.output_buffer = bytearray()
            self.input_buffer = []

    def read(self, n):
        self.data_available_event.wait()
        self.data_available_event.clear()

        with self.lock:
            result = self.output_buffer[self.index_read:self.index_read+n]
            self.index_read += n

            if self.index_read < len(self.output_buffer):
                self.data_available_event.set()

            return result

    def write(self, value):
        magic_code = value[:2]
        length = value[2:4]
        payload = value[4:]
        packet = Packet.extract(payload)

        self.input_buffer.append(packet)
        if packet.payload_type in self.callbacks:
            self.callbacks[packet.payload_type]()

    def on(self, payload_type, callback):
        self.callbacks[payload_type] = callback


class MockSubprocess(object):

    def __init__(self):
        self.stdout = MockStdIO(self)
        self.stdin = MockStdIO(self)
        self.running = False

        self.on_receive(Payload.Exit, self.stop)

    def simulate_data(self, data):
        self.stdout.send(data)

    def on_receive(self, payload_type, callback):
        self.stdin.on(payload_type, callback)

    def start(self):
        self.running = True
        ready_packet = build_packet(Payload.Ready)
        self.simulate_data(ready_packet)

    def stop(self):
        exit_packet = build_packet(Payload.Exit)
        self.simulate_data(exit_packet)
        self.running = False

    def reset(self):
        self.stdin.clear()
        self.stdout.clear()

    def poll(self):
        if self.running:
            return None
        else:
            return True
