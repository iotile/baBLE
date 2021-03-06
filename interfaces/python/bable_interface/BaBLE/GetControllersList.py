# automatically generated by the FlatBuffers compiler, do not modify

# namespace: BaBLE

import flatbuffers

class GetControllersList(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsGetControllersList(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = GetControllersList()
        x.Init(buf, n + offset)
        return x

    # GetControllersList
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # GetControllersList
    def Controllers(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            x = self._tab.Vector(o)
            x += flatbuffers.number_types.UOffsetTFlags.py_type(j) * 4
            x = self._tab.Indirect(x)
            from .Controller import Controller
            obj = Controller()
            obj.Init(self._tab.Bytes, x)
            return obj
        return None

    # GetControllersList
    def ControllersLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

def GetControllersListStart(builder): builder.StartObject(1)
def GetControllersListAddControllers(builder, controllers): builder.PrependUOffsetTRelativeSlot(0, flatbuffers.number_types.UOffsetTFlags.py_type(controllers), 0)
def GetControllersListStartControllersVector(builder, numElems): return builder.StartVector(4, numElems, 4)
def GetControllersListEnd(builder): return builder.EndObject()
