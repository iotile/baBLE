import threading
from bable_interface.BaBLE.Payload import Payload
from bable_interface.models import Packet
from bable_interface.utils import MAGIC_CODE


class ReceivingThread(threading.Thread):

    def __init__(self, stop_event, on_receive, file):
        super(ReceivingThread, self).__init__()
        self.stop_event = stop_event
        self.on_receive = on_receive
        self.file = file

    def run(self):
        try:
            while True:
                header = bytearray()
                while len(header) < 4:
                    header += self.file.read(1)

                if header[:2] != MAGIC_CODE:
                    print('Wrong magic code received: {}'.format(header[:2]))
                    continue

                length = (header[3] << 8) | header[2]  # depends on ENDIANNESS

                # Needs timeout
                payload = bytearray()
                while len(payload) < length:
                    payload += self.file.read(1)

                packet = Packet.extract(payload)
                if packet.payload_type == Payload.Exit:
                    break

                self.on_receive(packet)
        except Exception as e:
            print(e)
