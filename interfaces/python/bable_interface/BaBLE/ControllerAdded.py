# automatically generated by the FlatBuffers compiler, do not modify

# namespace: BaBLE

import flatbuffers

class ControllerAdded(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsControllerAdded(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = ControllerAdded()
        x.Init(buf, n + offset)
        return x

    # ControllerAdded
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

def ControllerAddedStart(builder): builder.StartObject(0)
def ControllerAddedEnd(builder): return builder.EndObject()