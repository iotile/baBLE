cmake_minimum_required(VERSION 3.5)
project(libuv)

set(LIBUVDIR libuv)

include_directories(${LIBUVDIR}/include ${LIBUVDIR}/src)

set(SOURCES
        ${LIBUVDIR}/include/uv.h
        ${LIBUVDIR}/include/uv/errno.h
        ${LIBUVDIR}/include/uv/linux.h
        ${LIBUVDIR}/include/uv/threadpool.h
        ${LIBUVDIR}/include/uv/tree.h
        ${LIBUVDIR}/include/uv/unix.h
        ${LIBUVDIR}/include/uv/version.h
        ${LIBUVDIR}/src/fs-poll.c
        ${LIBUVDIR}/src/heap-inl.h
        ${LIBUVDIR}/src/inet.c
        ${LIBUVDIR}/src/queue.h
        ${LIBUVDIR}/src/threadpool.c
        ${LIBUVDIR}/src/timer.c
        ${LIBUVDIR}/src/uv-common.c
        ${LIBUVDIR}/src/uv-common.h
        ${LIBUVDIR}/src/uv-data-getter-setters.c
        ${LIBUVDIR}/src/version.c
        ${LIBUVDIR}/src/unix/async.c
        ${LIBUVDIR}/src/unix/atomic-ops.h
        ${LIBUVDIR}/src/unix/core.c
        ${LIBUVDIR}/src/unix/dl.c
        ${LIBUVDIR}/src/unix/fs.c
        ${LIBUVDIR}/src/unix/getaddrinfo.c
        ${LIBUVDIR}/src/unix/getnameinfo.c
        ${LIBUVDIR}/src/unix/internal.h
        ${LIBUVDIR}/src/unix/linux-core.c
        ${LIBUVDIR}/src/unix/linux-inotify.c
        ${LIBUVDIR}/src/unix/linux-syscalls.c
        ${LIBUVDIR}/src/unix/linux-syscalls.h
        ${LIBUVDIR}/src/unix/loop.c
        ${LIBUVDIR}/src/unix/loop-watcher.c
        ${LIBUVDIR}/src/unix/no-fsevents.c
        ${LIBUVDIR}/src/unix/no-proctitle.c
        ${LIBUVDIR}/src/unix/pipe.c
        ${LIBUVDIR}/src/unix/poll.c
        ${LIBUVDIR}/src/unix/process.c
        ${LIBUVDIR}/src/unix/procfs-exepath.c
        ${LIBUVDIR}/src/unix/proctitle.c
        ${LIBUVDIR}/src/unix/signal.c
        ${LIBUVDIR}/src/unix/spinlock.h
        ${LIBUVDIR}/src/unix/stream.c
        ${LIBUVDIR}/src/unix/sysinfo-loadavg.c
        ${LIBUVDIR}/src/unix/sysinfo-memory.c
        ${LIBUVDIR}/src/unix/tcp.c
        ${LIBUVDIR}/src/unix/thread.c
        ${LIBUVDIR}/src/unix/tty.c
        ${LIBUVDIR}/src/unix/udp.c
        )

add_library(uv ${SOURCES})

find_package(Threads)
target_link_libraries(uv ${CMAKE_THREAD_LIBS_INIT})
