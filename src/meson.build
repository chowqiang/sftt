# Copyright (C)  2020-2021 Min Zhou <zhoumin@bupt.cn>, all rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

sources = files(
	'bits.c',
	'btree.c',
	'client.c',
	'cmdline.c',
	'command.c',
	'compress.c',
	'config.c',
	'connect.c',
	'context.c',
	'db.c',
	'debug.c',
	'destroy.c',
	'dir.c',
	'dlist.c',
	'encode.c',
	'encrypt.c',
	'endpoint.c',
	'file.c',
	'file_recv.c',
	'file_send.c',
	'lock.c',
	'log.c',
	'map.c',
	'md5.c',
	'mem_pool.c',
	'mkdirp.c',
	'msg_queue.c',
	'msq_thread.c',
	'net_trans.c',
	'option.c',
	'packet.c',
	'path-normalize.c',
	'progress_viewer.c',
	'queue.c',
	'ratelimit.c',
	'req_resp.c',
	'response.c',
	'rte_errno.c',
	'sds.c',
	'serialize_handler.c',
	'server.c',
	'session.c',
	'shm_space.c',
	'show.c',
	'stack.c',
	'strtox.c',
	'test_common.c',
	'thread_pool.c',
	'trace.c',
	'user.c',
	'utils.c',
	'validate.c',
	'version.c',
)

lib_src += sources

sftt_lib += static_library('sftt', lib_src, include_directories : sftt_inc, dependencies: gen_deps)

lib_deps = declare_dependency(link_with : sftt_lib, include_directories: sftt_inc)
