import logging
import threading
import sys
from bable_interface.BaBLE.Payload import Payload
from bable_interface.models import Packet
from bable_interface.utils import MAGIC_CODE


class ReceivingThread(threading.Thread):

    def __init__(self, on_receive, file):
        super(ReceivingThread, self).__init__(name=__name__)
        self.logger = logging.getLogger(__name__)

        self.on_receive = on_receive
        self.file = file

    def run(self):
        while True:
            header = bytearray()
            while len(header) < 4:
                header += self.file.read(1)

            if header[:2] != MAGIC_CODE:
                self.logger.error("Wrong magic code received: %s", header[:2])
                continue

            if sys.byteorder == 'little':  # depends on ENDIANNESS
                length = (header[3] << 8) | header[2]
            else:
                length = (header[2] << 8) | header[3]

            payload = bytearray()
            while len(payload) < length:
                payload += self.file.read(1)

            packet = Packet.extract(payload)
            if packet.payload_type == Payload.Exit:
                break

            self.on_receive(packet)
