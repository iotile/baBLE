import functools
import threading
import sys

PYTHON_2 = sys.version_info[0] < 3

if PYTHON_2:
    import trollius as asyncio
else:
    import asyncio


class WorkingThread(threading.Thread):

    def __init__(self, ready_event):
        super(WorkingThread, self).__init__()
        self.loop = asyncio.get_event_loop()
        self.thread_id = None
        self.ready_event = ready_event

    def run(self):
        self.loop = asyncio.new_event_loop()
        asyncio.set_event_loop(self.loop)
        self.thread_id = threading.current_thread()
        self.loop.call_soon(self.ready_event.set)
        self.loop.run_forever()

    def stop(self):
        self.loop.call_soon_threadsafe(self.loop.stop)

    def add_task(self, task, callback=None):
        def _async_add(func):
            try:
                result = func()
                if callback is not None:
                    result.add_done_callback(callback)
            except Exception as e:
                print(e)

        func = functools.partial(asyncio.ensure_future, task, loop=self.loop)
        if threading.current_thread() == self.thread_id:
            result = func()  # We can call directly if we're not going between threads.
            if callback is not None:
                result.add_done_callback(callback)
        else:
            self.loop.call_soon_threadsafe(_async_add, func)

    def cancel_task(self, task):
        self.loop.call_soon_threadsafe(task.cancel)
