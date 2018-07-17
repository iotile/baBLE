# automatically generated by the FlatBuffers compiler, do not modify

# namespace: BaBLE

import flatbuffers

class SetGATTTable(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsSetGATTTable(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = SetGATTTable()
        x.Init(buf, n + offset)
        return x

    # SetGATTTable
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # SetGATTTable
    def Services(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            x = self._tab.Vector(o)
            x += flatbuffers.number_types.UOffsetTFlags.py_type(j) * 4
            x = self._tab.Indirect(x)
            from .Service import Service
            obj = Service()
            obj.Init(self._tab.Bytes, x)
            return obj
        return None

    # SetGATTTable
    def ServicesLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

    # SetGATTTable
    def Characteristics(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            x = self._tab.Vector(o)
            x += flatbuffers.number_types.UOffsetTFlags.py_type(j) * 4
            x = self._tab.Indirect(x)
            from .Characteristic import Characteristic
            obj = Characteristic()
            obj.Init(self._tab.Bytes, x)
            return obj
        return None

    # SetGATTTable
    def CharacteristicsLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

def SetGATTTableStart(builder): builder.StartObject(2)
def SetGATTTableAddServices(builder, services): builder.PrependUOffsetTRelativeSlot(0, flatbuffers.number_types.UOffsetTFlags.py_type(services), 0)
def SetGATTTableStartServicesVector(builder, numElems): return builder.StartVector(4, numElems, 4)
def SetGATTTableAddCharacteristics(builder, characteristics): builder.PrependUOffsetTRelativeSlot(1, flatbuffers.number_types.UOffsetTFlags.py_type(characteristics), 0)
def SetGATTTableStartCharacteristicsVector(builder, numElems): return builder.StartVector(4, numElems, 4)
def SetGATTTableEnd(builder): return builder.EndObject()
