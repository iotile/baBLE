# automatically generated by the FlatBuffers compiler, do not modify

# namespace: BaBLE

import flatbuffers

class Discovering(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsDiscovering(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = Discovering()
        x.Init(buf, n + offset)
        return x

    # Discovering
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # Discovering
    def AddressType(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint8Flags, o + self._tab.Pos)
        return 0

    # Discovering
    def State(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return bool(self._tab.Get(flatbuffers.number_types.BoolFlags, o + self._tab.Pos))
        return False

def DiscoveringStart(builder): builder.StartObject(2)
def DiscoveringAddAddressType(builder, addressType): builder.PrependUint8Slot(0, addressType, 0)
def DiscoveringAddState(builder, state): builder.PrependBoolSlot(1, state, 0)
def DiscoveringEnd(builder): return builder.EndObject()