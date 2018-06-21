import functools
import threading
import sys

if sys.version_info < (3, 4):
    import trollius as asyncio
else:
    import asyncio


class WorkingThread(threading.Thread):

    def __init__(self, ready_event):
        super(WorkingThread, self).__init__(name=__name__)
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
        current_task = asyncio.Task.current_task()
        for task in asyncio.Task.all_tasks():
            if current_task is None or task != current_task:
                self.cancel_task(task)
        # FIXME: error when stopping from working thread (cancelled coroutine still pending)
        self.loop.call_soon_threadsafe(self.loop.stop)

    def add_task(self, task, callback=None, **kwargs):
        def run_task(func):
            result = func()
            if callback is not None:
                result.add_done_callback(functools.partial(callback, **kwargs))

        async_task = functools.partial(asyncio.ensure_future, task, loop=self.loop)

        if threading.current_thread() == self.thread_id:  # We can call directly if we're not going between threads.
            run_task(async_task)
        else:
            self.loop.call_soon_threadsafe(run_task, async_task)

    def cancel_task(self, task):
        self.loop.call_soon_threadsafe(task.cancel)
