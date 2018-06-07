import bable_interface
import time


def on_device_found(device):
    print("ON DEVICE FOUND", device)


bable = bable_interface.BaBLE()
bable.start()
bable.connect("00:11:22:33:44:55")
print("CONNECTED")
# bable.start_scan(on_device_found)
# print("SCAN STARTED IN TEST.PY")
# time.sleep(2)
# bable.stop_scan()
# print("SCAN STOPPED IN TEST.PY")
time.sleep(2)
bable.stop()
print("DONE")

# import time
# import asyncio
# import functools
# from threading import Thread, current_thread, Event
# from concurrent.futures import Future
#
#
# class B(Thread):
#     def __init__(self, start_event):
#         Thread.__init__(self)
#         self.loop = None
#         self.tid = None
#         self.event = start_event
#
#     def run(self):
#         self.loop = asyncio.new_event_loop()
#         asyncio.set_event_loop(self.loop)
#         self.tid = current_thread()
#         self.loop.call_soon(self.event.set)
#         self.loop.run_forever()
#
#     def stop(self):
#         self.loop.call_soon_threadsafe(self.loop.stop)
#
#     def add_task(self, coro):
#         """this method should return a task object, that I
#           can cancel, not a handle"""
#         def _async_add(func, fut):
#             try:
#                 ret = func()
#                 ret.add_done_callback(lambda res: fut.set_result(res.result()))
#             except Exception as e:
#                 fut.set_exception(e)
#
#         f = functools.partial(asyncio.async, coro, loop=self.loop)
#         if current_thread() == self.tid:
#             return f()  # We can call directly if we're not going between threads.
#         else:
#             # We're in a non-event loop thread so we use a Future
#             # to get the task from the event loop thread once
#             # it's ready.
#             fut = Future()
#             self.loop.call_soon_threadsafe(_async_add, f, fut)
#             return fut.result()
#
#     def cancel_task(self, task):
#         self.loop.call_soon_threadsafe(task.cancel)
#
#
# @asyncio.coroutine
# def test():
#     yield from asyncio.sleep(1)
#     print("running")
#     return "OK"
#
#
# event = Event()
# b = B(event)
# b.start()
# event.wait()  # Let the loop's thread signal us, rather than sleeping
# print("READY")
# t = b.add_task(test())  # This is a real task
# print(t)
# b.stop()
