import asyncio


@asyncio.coroutine
def connect(address):
    print("CALLED CONNECT PY3")
    return "connect py3 ({})".format(address)
