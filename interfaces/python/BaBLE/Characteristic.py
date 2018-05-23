# automatically generated by the FlatBuffers compiler, do not modify

# namespace: BaBLE

import flatbuffers

class Characteristic(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsCharacteristic(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = Characteristic()
        x.Init(buf, n + offset)
        return x

    # Characteristic
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # Characteristic
    def Handle(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint16Flags, o + self._tab.Pos)
        return 0

    # Characteristic
    def ValueHandle(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint16Flags, o + self._tab.Pos)
        return 0

    # Characteristic
    def Indicate(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(8))
        if o != 0:
            return bool(self._tab.Get(flatbuffers.number_types.BoolFlags, o + self._tab.Pos))
        return False

    # Characteristic
    def Notify(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(10))
        if o != 0:
            return bool(self._tab.Get(flatbuffers.number_types.BoolFlags, o + self._tab.Pos))
        return False

    # Characteristic
    def Write(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(12))
        if o != 0:
            return bool(self._tab.Get(flatbuffers.number_types.BoolFlags, o + self._tab.Pos))
        return False

    # Characteristic
    def Read(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(14))
        if o != 0:
            return bool(self._tab.Get(flatbuffers.number_types.BoolFlags, o + self._tab.Pos))
        return False

    # Characteristic
    def Broadcast(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(16))
        if o != 0:
            return bool(self._tab.Get(flatbuffers.number_types.BoolFlags, o + self._tab.Pos))
        return False

    # Characteristic
    def Uuid(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(18))
        if o != 0:
            return self._tab.String(o + self._tab.Pos)
        return None

def CharacteristicStart(builder): builder.StartObject(8)
def CharacteristicAddHandle(builder, handle): builder.PrependUint16Slot(0, handle, 0)
def CharacteristicAddValueHandle(builder, valueHandle): builder.PrependUint16Slot(1, valueHandle, 0)
def CharacteristicAddIndicate(builder, indicate): builder.PrependBoolSlot(2, indicate, 0)
def CharacteristicAddNotify(builder, notify): builder.PrependBoolSlot(3, notify, 0)
def CharacteristicAddWrite(builder, write): builder.PrependBoolSlot(4, write, 0)
def CharacteristicAddRead(builder, read): builder.PrependBoolSlot(5, read, 0)
def CharacteristicAddBroadcast(builder, broadcast): builder.PrependBoolSlot(6, broadcast, 0)
def CharacteristicAddUuid(builder, uuid): builder.PrependUOffsetTRelativeSlot(7, flatbuffers.number_types.UOffsetTFlags.py_type(uuid), 0)
def CharacteristicEnd(builder): return builder.EndObject()
