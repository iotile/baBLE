# automatically generated by the FlatBuffers compiler, do not modify

# namespace: BaBLE

import flatbuffers

class ControllerRemoved(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsControllerRemoved(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = ControllerRemoved()
        x.Init(buf, n + offset)
        return x

    # ControllerRemoved
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

def ControllerRemovedStart(builder): builder.StartObject(0)
def ControllerRemovedEnd(builder): return builder.EndObject()
