/**@file
 * This is the internal header file of the stream socket server declarations.
 *
 * @copyright 2019 Lely Industries N.V.
 *
 * @author J. S. Seldenthuis <jseldenthuis@lely.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LELY_IO2_INTERN_SYS_SOCK_STREAM_SRV_H_
#define LELY_IO2_INTERN_SYS_SOCK_STREAM_SRV_H_

#include "../io2.h"

#if _WIN32 || (defined(_POSIX_C_SOURCE) && !defined(__NEWLIB__))

#include "sock.h"
#if !LELY_NO_THREADS
#include <lely/libc/threads.h>
#endif
#include <lely/io2/ctx.h>
#ifdef _POSIX_C_SOURCE
#include <lely/io2/posix/poll.h>
#endif
#include <lely/io2/sock_stream_srv.h>

#if _WIN32
#include <mswsock.h>
#endif

/**
 * A struct containing the native socket handle or file descriptor and
 * associated attributes for a stream socket server.
 */
struct io_sock_stream_srv_handle {
	/// The native socket handle or file descriptor.
	SOCKET fd;
#if _WIN32
	/// The base service provider handle of #fd (see io_wsa_base_handle()).
	SOCKET base;
#endif
	/// The native address family of #fd.
	int family;
	/// The native protocol of #fd.
	int protocol;
#if _WIN32
	/// A pointer to the AcceptEx() function.
	LPFN_ACCEPTEX lpfnAcceptEx;
	/**
	 * If an I/O operation completes synchronously and this flag is set, no
	 * completion packet is posted.
	 */
	int skip_iocp;
#endif
};

/// The static initializer for #io_sock_stream_srv_handle.
#if _WIN32
#define IO_SOCK_STREAM_SRV_HANDLE_INIT \
	{ \
		INVALID_SOCKET, INVALID_SOCKET, AF_UNSPEC, 0, NULL, 0 \
	}
#else
#define IO_SOCK_STREAM_SRV_HANDLE_INIT \
	{ \
		-1, AF_UNSPEC, 0 \
	}
#endif

/// The implementation of a stream socket server.
struct io_sock_stream_srv_impl {
	/// A pointer to the virtual table for the I/O device interface.
	const struct io_dev_vtbl *dev_vptr;
	/// A pointer to the virtual table for the socket interface.
	const struct io_sock_vtbl *sock_vptr;
	/**
	 * A pointer to the virtual table for the stream socket server
	 * interface.
	 */
	const struct io_sock_stream_srv_vtbl *sock_stream_srv_vptr;
	/**
	 * A pointer to the virtual table containing network protocol endpoint
	 * conversion functions.
	 */
	const struct io_endp_vtbl *endp_vptr;
	/**
	 * A pointer to the polling instance used to watch for I/O events (on
	 * POSIX platforms) or completion packes (on Windows). If <b>poll</b> is
	 * NULL, operations are performed in blocking mode and the executor is
	 * used as a worker thread.
	 */
	io_poll_t *poll;
	/// The I/O service representing the socket.
	struct io_svc svc;
	/// A pointer to the I/O context with which the socket is registered.
	io_ctx_t *ctx;
	/// A pointer to the executor used to execute all I/O tasks.
	ev_exec_t *exec;
#ifdef _POSIX_C_SOURCE
	/// The object used to monitor the file descriptor for I/O events.
	struct io_poll_watch watch;
#endif
	/// The task responsible for intiating I/O event wait operations.
	struct ev_task wait_task;
	/// The task responsible for intiating accept operations.
	struct ev_task accept_task;
#if !LELY_NO_THREADS
	/**
	 * The mutex protecting the socket handle or file descriptor and the
	 * queues of pending operations.
	 */
	mtx_t mtx;
#endif
	/**
	 * A struct containing the native socket handle or file descriptor and
	 * associated attributes.
	 */
	struct io_sock_stream_srv_handle handle;
	/// A flag indicating whether the I/O service has been shut down.
	unsigned shutdown : 1;
	/// A flag indicating whether #wait_task has been posted to #exec.
	unsigned wait_posted : 1;
	/// A flag indicating whether #accept_task has been posted to #exec.
	unsigned accept_posted : 1;
	/// The queue containing pending I/O event wait operations.
	struct sllist wait_queue;
#if _WIN32
	/**
	 * The queue containing successfully initiated I/O event wait operations
	 * waiting for a completion packet.
	 */
	struct sllist wait_iocp_queue;
#endif
	/// The queue containing pending accept operations.
	struct sllist accept_queue;
	/// The accept operation currently being executed.
	struct ev_task *current_accept;
#if _WIN32
	/**
	 * The queue containing successfully initiated accept operations waiting
	 * for a completion packet.
	 */
	struct sllist accept_iocp_queue;
#endif
};

#ifdef __cplusplus
extern "C" {
#endif

int io_sock_stream_srv_impl_init(struct io_sock_stream_srv_impl *impl,
		io_poll_t *poll, ev_exec_t *exec,
		const struct io_endp_vtbl *endp_vptr);
void io_sock_stream_srv_impl_fini(struct io_sock_stream_srv_impl *impl);

void io_sock_stream_srv_impl_get_handle(
		const struct io_sock_stream_srv_impl *impl,
		struct io_sock_stream_srv_handle *phandle);
SOCKET io_sock_stream_srv_impl_open(
		struct io_sock_stream_srv_impl *impl, int family, int protocol);
int io_sock_stream_srv_impl_assign(struct io_sock_stream_srv_impl *impl,
		const struct io_sock_stream_srv_handle *handle);
SOCKET io_sock_stream_srv_impl_release(struct io_sock_stream_srv_impl *impl);

#ifdef __cplusplus
}
#endif

#endif // _WIN32 || (_POSIX_C_SOURCE && !__NEWLIB__)

#endif // !LELY_IO2_INTERN_SYS_SOCK_STREAM_SRV_H_