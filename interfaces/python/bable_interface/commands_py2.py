import trollius as asyncio


@asyncio.coroutine
def connect(address):
    print("CALLED CONNECT PY2")
    return "connect py2 ({})".format(address)
