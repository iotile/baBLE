import functools
import logging
import threading
import sys

if sys.version_info < (3, 4):
    import trollius as asyncio
else:
    import asyncio


class WorkingThread(threading.Thread):

    def __init__(self, ready_event):
        super(WorkingThread, self).__init__(name=__name__)
        self.logger = logging.getLogger(__name__)
        self.loop = asyncio.get_event_loop()
        self.thread_id = None
        self.ready_event = ready_event

    def run(self):
        self.loop = asyncio.new_event_loop()
        asyncio.set_event_loop(self.loop)
        self.thread_id = threading.current_thread()
        self.loop.call_soon(self.ready_event.set)
        self.loop.run_forever()

    def stop(self, sync):
        def on_stopped(fut, event):
            event.set()

        done_event = threading.Event()
        self.add_task(self._async_stop(), callback=on_stopped, event=done_event)

        if sync:
            result = done_event.wait(timeout=5.0)
            if not result:
                self.logger.warning("Working thread will not be stopped properly...")

    @asyncio.coroutine
    def _async_stop(self):
        current_task = asyncio.Task.current_task(loop=self.loop)
        for task in asyncio.Task.all_tasks(loop=self.loop):
            if current_task is None or task != current_task:
                task.cancel()
        self.loop.call_soon(self.loop.stop)

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
