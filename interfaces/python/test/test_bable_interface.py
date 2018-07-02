import bable_interface
from bable_interface.flatbuffer import build_packet
from bable_interface.BaBLE.Payload import Payload
import subprocess


class IOMock(object):

    def __init__(self):
        self.nb_read = 0
        self.packet = build_packet(Payload.Ready, params={'native_class': 'BaBLE'})  # TODO: make it send native class

    def read(self, n):
        print("READ", self.nb_read)
        if self.nb_read % 2 == 0:
            result = self.packet[:4]
        else:
            result = self.packet[4:]

        self.nb_read += 1
        return result

    def write(self, value):
        return None


class SubprocessMock(object):

    def __init__(self):
        self.stdout = IOMock()
        self.stdin = IOMock()

    def poll(self):
        return True


def test_start_stop(monkeypatch):
    monkeypatch.setattr(subprocess, 'Popen', lambda cmd, *args, **kwargs: SubprocessMock())
    bable = bable_interface.BaBLEInterface()
    bable.start()
    bable.stop()
