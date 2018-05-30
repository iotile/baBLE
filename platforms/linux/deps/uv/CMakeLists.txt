cmake_minimum_required(VERSION 3.10)
project(libuv)

set(CMAKE_CXX_FLAGS "-funsigned-char")
set(LIBUVDIR libuv)

include_directories(${LIBUVDIR}/include ${LIBUVDIR}/src)

set(SOURCES
        ${LIBUVDIR}/include/uv.h
        ${LIBUVDIR}/include/tree.h
        ${LIBUVDIR}/include/uv-errno.h
        ${LIBUVDIR}/include/uv-threadpool.h
        ${LIBUVDIR}/include/uv-version.h
        ${LIBUVDIR}/src/fs-poll.c
        ${LIBUVDIR}/src/heap-inl.h
        ${LIBUVDIR}/src/inet.c
        ${LIBUVDIR}/src/queue.h
        ${LIBUVDIR}/src/threadpool.c
        ${LIBUVDIR}/src/uv-common.c
        ${LIBUVDIR}/src/uv-common.h
        ${LIBUVDIR}/src/version.c
        ${LIBUVDIR}/include/uv-unix.h
        ${LIBUVDIR}/include/uv-linux.h
        ${LIBUVDIR}/include/uv-sunos.h
        ${LIBUVDIR}/include/uv-darwin.h
        ${LIBUVDIR}/include/uv-bsd.h
        ${LIBUVDIR}/include/uv-aix.h
        ${LIBUVDIR}/src/unix/async.c
        ${LIBUVDIR}/src/unix/atomic-ops.h
        ${LIBUVDIR}/src/unix/core.c
        ${LIBUVDIR}/src/unix/dl.c
        ${LIBUVDIR}/src/unix/fs.c
        ${LIBUVDIR}/src/unix/getaddrinfo.c
        ${LIBUVDIR}/src/unix/getnameinfo.c
        ${LIBUVDIR}/src/unix/internal.h
        ${LIBUVDIR}/src/unix/loop.c
        ${LIBUVDIR}/src/unix/loop-watcher.c
        ${LIBUVDIR}/src/unix/pipe.c
        ${LIBUVDIR}/src/unix/poll.c
        ${LIBUVDIR}/src/unix/process.c
        ${LIBUVDIR}/src/unix/signal.c
        ${LIBUVDIR}/src/unix/spinlock.h
        ${LIBUVDIR}/src/unix/stream.c
        ${LIBUVDIR}/src/unix/tcp.c
        ${LIBUVDIR}/src/unix/thread.c
        ${LIBUVDIR}/src/unix/timer.c
        ${LIBUVDIR}/src/unix/tty.c
        ${LIBUVDIR}/src/unix/udp.c
        ${LIBUVDIR}/src/unix/proctitle.c
        ${LIBUVDIR}/src/unix/linux-core.c
        ${LIBUVDIR}/src/unix/linux-inotify.c
        ${LIBUVDIR}/src/unix/linux-syscalls.c
        ${LIBUVDIR}/src/unix/linux-syscalls.h
        )

add_library(uv ${SOURCES})

find_package(Threads)
target_link_libraries(uv ${CMAKE_THREAD_LIBS_INIT})
