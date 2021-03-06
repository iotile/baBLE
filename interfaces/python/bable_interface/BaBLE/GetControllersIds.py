# automatically generated by the FlatBuffers compiler, do not modify

# namespace: BaBLE

import flatbuffers

class GetControllersIds(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsGetControllersIds(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = GetControllersIds()
        x.Init(buf, n + offset)
        return x

    # GetControllersIds
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # GetControllersIds
    def ControllersIds(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            a = self._tab.Vector(o)
            return self._tab.Get(flatbuffers.number_types.Uint16Flags, a + flatbuffers.number_types.UOffsetTFlags.py_type(j * 2))
        return 0

    # GetControllersIds
    def ControllersIdsAsNumpy(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.GetVectorAsNumpy(flatbuffers.number_types.Uint16Flags, o)
        return 0

    # GetControllersIds
    def ControllersIdsLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

def GetControllersIdsStart(builder): builder.StartObject(1)
def GetControllersIdsAddControllersIds(builder, controllersIds): builder.PrependUOffsetTRelativeSlot(0, flatbuffers.number_types.UOffsetTFlags.py_type(controllersIds), 0)
def GetControllersIdsStartControllersIdsVector(builder, numElems): return builder.StartVector(2, numElems, 2)
def GetControllersIdsEnd(builder): return builder.EndObject()
