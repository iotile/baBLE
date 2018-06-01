# automatically generated by the FlatBuffers compiler, do not modify

# namespace: BaBLE

import flatbuffers

class DeviceConnected(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsDeviceConnected(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = DeviceConnected()
        x.Init(buf, n + offset)
        return x

    # DeviceConnected
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # DeviceConnected
    def ConnectionHandle(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint16Flags, o + self._tab.Pos)
        return 0

    # DeviceConnected
    def Address(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return self._tab.String(o + self._tab.Pos)
        return None

    # DeviceConnected
    def AddressType(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(8))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint8Flags, o + self._tab.Pos)
        return 0

    # DeviceConnected
    def Flags(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(10))
        if o != 0:
            a = self._tab.Vector(o)
            return self._tab.Get(flatbuffers.number_types.Uint8Flags, a + flatbuffers.number_types.UOffsetTFlags.py_type(j * 1))
        return 0

    # DeviceConnected
    def FlagsAsNumpy(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(10))
        if o != 0:
            return self._tab.GetVectorAsNumpy(flatbuffers.number_types.Uint8Flags, o)
        return 0

    # DeviceConnected
    def FlagsLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(10))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

    # DeviceConnected
    def Uuid(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(12))
        if o != 0:
            return self._tab.String(o + self._tab.Pos)
        return None

    # DeviceConnected
    def CompanyId(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(14))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint16Flags, o + self._tab.Pos)
        return 0

    # DeviceConnected
    def ManufacturerDataAdvertised(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(16))
        if o != 0:
            a = self._tab.Vector(o)
            return self._tab.Get(flatbuffers.number_types.Uint8Flags, a + flatbuffers.number_types.UOffsetTFlags.py_type(j * 1))
        return 0

    # DeviceConnected
    def ManufacturerDataAdvertisedAsNumpy(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(16))
        if o != 0:
            return self._tab.GetVectorAsNumpy(flatbuffers.number_types.Uint8Flags, o)
        return 0

    # DeviceConnected
    def ManufacturerDataAdvertisedLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(16))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

    # DeviceConnected
    def ManufacturerDataScanned(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(18))
        if o != 0:
            a = self._tab.Vector(o)
            return self._tab.Get(flatbuffers.number_types.Uint8Flags, a + flatbuffers.number_types.UOffsetTFlags.py_type(j * 1))
        return 0

    # DeviceConnected
    def ManufacturerDataScannedAsNumpy(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(18))
        if o != 0:
            return self._tab.GetVectorAsNumpy(flatbuffers.number_types.Uint8Flags, o)
        return 0

    # DeviceConnected
    def ManufacturerDataScannedLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(18))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

    # DeviceConnected
    def DeviceName(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(20))
        if o != 0:
            return self._tab.String(o + self._tab.Pos)
        return None

def DeviceConnectedStart(builder): builder.StartObject(9)
def DeviceConnectedAddConnectionHandle(builder, connectionHandle): builder.PrependUint16Slot(0, connectionHandle, 0)
def DeviceConnectedAddAddress(builder, address): builder.PrependUOffsetTRelativeSlot(1, flatbuffers.number_types.UOffsetTFlags.py_type(address), 0)
def DeviceConnectedAddAddressType(builder, addressType): builder.PrependUint8Slot(2, addressType, 0)
def DeviceConnectedAddFlags(builder, flags): builder.PrependUOffsetTRelativeSlot(3, flatbuffers.number_types.UOffsetTFlags.py_type(flags), 0)
def DeviceConnectedStartFlagsVector(builder, numElems): return builder.StartVector(1, numElems, 1)
def DeviceConnectedAddUuid(builder, uuid): builder.PrependUOffsetTRelativeSlot(4, flatbuffers.number_types.UOffsetTFlags.py_type(uuid), 0)
def DeviceConnectedAddCompanyId(builder, companyId): builder.PrependUint16Slot(5, companyId, 0)
def DeviceConnectedAddManufacturerDataAdvertised(builder, manufacturerDataAdvertised): builder.PrependUOffsetTRelativeSlot(6, flatbuffers.number_types.UOffsetTFlags.py_type(manufacturerDataAdvertised), 0)
def DeviceConnectedStartManufacturerDataAdvertisedVector(builder, numElems): return builder.StartVector(1, numElems, 1)
def DeviceConnectedAddManufacturerDataScanned(builder, manufacturerDataScanned): builder.PrependUOffsetTRelativeSlot(7, flatbuffers.number_types.UOffsetTFlags.py_type(manufacturerDataScanned), 0)
def DeviceConnectedStartManufacturerDataScannedVector(builder, numElems): return builder.StartVector(1, numElems, 1)
def DeviceConnectedAddDeviceName(builder, deviceName): builder.PrependUOffsetTRelativeSlot(8, flatbuffers.number_types.UOffsetTFlags.py_type(deviceName), 0)
def DeviceConnectedEnd(builder): return builder.EndObject()