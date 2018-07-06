cmake_minimum_required(VERSION 3.5)
project(libuv)

set(LIBUVDIR libuv)
set(INCLUDEDIR ${LIBUVDIR}/include)
set(SRCDIR ${LIBUVDIR}/src)

include_directories(${INCLUDEDIR} ${SRCDIR})

set(SOURCES
        ${SRCDIR}/fs-poll.c
        ${SRCDIR}/heap-inl.h
        ${SRCDIR}/inet.c
        ${SRCDIR}/queue.h
        ${SRCDIR}/threadpool.c
        ${SRCDIR}/timer.c
        ${SRCDIR}/uv-common.c
        ${SRCDIR}/uv-common.h
        ${SRCDIR}/uv-data-getter-setters.c
        ${SRCDIR}/version.c
        ${SRCDIR}/unix/async.c
        ${SRCDIR}/unix/atomic-ops.h
        ${SRCDIR}/unix/core.c
        ${SRCDIR}/unix/dl.c
        ${SRCDIR}/unix/fs.c
        ${SRCDIR}/unix/getaddrinfo.c
        ${SRCDIR}/unix/getnameinfo.c
        ${SRCDIR}/unix/internal.h
        ${SRCDIR}/unix/linux-core.c
        ${SRCDIR}/unix/linux-inotify.c
        ${SRCDIR}/unix/linux-syscalls.c
        ${SRCDIR}/unix/linux-syscalls.h
        ${SRCDIR}/unix/loop.c
        ${SRCDIR}/unix/loop-watcher.c
        ${SRCDIR}/unix/no-fsevents.c
        ${SRCDIR}/unix/no-proctitle.c
        ${SRCDIR}/unix/pipe.c
        ${SRCDIR}/unix/poll.c
        ${SRCDIR}/unix/process.c
        ${SRCDIR}/unix/procfs-exepath.c
        ${SRCDIR}/unix/proctitle.c
        ${SRCDIR}/unix/signal.c
        ${SRCDIR}/unix/spinlock.h
        ${SRCDIR}/unix/stream.c
        ${SRCDIR}/unix/sysinfo-loadavg.c
        ${SRCDIR}/unix/sysinfo-memory.c
        ${SRCDIR}/unix/tcp.c
        ${SRCDIR}/unix/thread.c
        ${SRCDIR}/unix/tty.c
        ${SRCDIR}/unix/udp.c
        )

add_library(uv ${SOURCES})

find_package(Threads)
target_link_libraries(uv ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(uv PUBLIC ${INCLUDEDIR})
