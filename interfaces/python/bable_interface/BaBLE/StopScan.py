# automatically generated by the FlatBuffers compiler, do not modify

# namespace: BaBLE

import flatbuffers

class StopScan(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsStopScan(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = StopScan()
        x.Init(buf, n + offset)
        return x

    # StopScan
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

def StopScanStart(builder): builder.StartObject(0)
def StopScanEnd(builder): return builder.EndObject()
