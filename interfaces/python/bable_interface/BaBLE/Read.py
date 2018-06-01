# automatically generated by the FlatBuffers compiler, do not modify

# namespace: BaBLE

import flatbuffers

class Read(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsRead(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = Read()
        x.Init(buf, n + offset)
        return x

    # Read
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # Read
    def ConnectionHandle(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint16Flags, o + self._tab.Pos)
        return 0

    # Read
    def AttributeHandle(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint16Flags, o + self._tab.Pos)
        return 0

    # Read
    def Value(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(8))
        if o != 0:
            a = self._tab.Vector(o)
            return self._tab.Get(flatbuffers.number_types.Uint8Flags, a + flatbuffers.number_types.UOffsetTFlags.py_type(j * 1))
        return 0

    # Read
    def ValueAsNumpy(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(8))
        if o != 0:
            return self._tab.GetVectorAsNumpy(flatbuffers.number_types.Uint8Flags, o)
        return 0

    # Read
    def ValueLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(8))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

def ReadStart(builder): builder.StartObject(3)
def ReadAddConnectionHandle(builder, connectionHandle): builder.PrependUint16Slot(0, connectionHandle, 0)
def ReadAddAttributeHandle(builder, attributeHandle): builder.PrependUint16Slot(1, attributeHandle, 0)
def ReadAddValue(builder, value): builder.PrependUOffsetTRelativeSlot(2, flatbuffers.number_types.UOffsetTFlags.py_type(value), 0)
def ReadStartValueVector(builder, numElems): return builder.StartVector(1, numElems, 1)
def ReadEnd(builder): return builder.EndObject()