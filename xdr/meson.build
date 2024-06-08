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

rpcgen = find_program('rpcgen')
gen_req_resp_hdr = custom_target('req_resp.h',
			input: ['req_resp.x'],
			output: ['req_resp.h'],
			command: [rpcgen, '-C', '-h', '@INPUT@',
				'-o', '@OUTPUT@'],
			install: true,
			install_dir: ''
)
lib_src += gen_req_resp_hdr
gen_deps += declare_dependency(sources: gen_req_resp_hdr)

gen_req_resp_code = custom_target('req_resp.c',
			input: ['req_resp.x'],
                        output: ['req_resp.c'],
			command: [rpcgen, '-C', '-c', '@INPUT@',
				'-o', '@OUTPUT@'],
			install: true,
			install_dir: ''
)
lib_src += gen_req_resp_code
gen_deps += declare_dependency(sources: gen_req_resp_code)

gen_serialize = executable('gen_serialize', 'gen_serialize.c')
gen_serialize_hdr_code = custom_target('serialize.h.c',
				input: ['req_resp.x'],
				output: ['serialize.h', 'serialize.c'],
				command: [gen_serialize,
					'@INPUT@',
					'@OUTPUT0@',
					'@OUTPUT1@'],
				install: true,
				install_dir: ''
)
lib_src += gen_serialize_hdr_code
gen_deps += declare_dependency(sources: gen_serialize_hdr_code)

gen_serialize_raw = executable('gen_serialize_raw', 'gen_serialize_raw.c')
gen_serialize_raw_hdr_code = custom_target('serialize_raw.h.c',
				input: ['req_resp.x'],
				output: ['serialize_raw.h', 'serialize_raw.c'],
				command: [gen_serialize_raw,
					'@INPUT@',
					'@OUTPUT0@',
					'@OUTPUT1@'],
				install: true,
				install_dir: ''
)
lib_src += gen_serialize_raw_hdr_code
gen_deps += declare_dependency(sources: gen_serialize_raw_hdr_code)

gen_trans = executable('gen_trans', 'gen_trans.c')
gen_trans_hdr_code = custom_target('trans.h.c',
			input: ['req_resp.x'],
			output: ['trans.h', 'trans.c'],
			command: [gen_trans, '@INPUT@',
				'@OUTPUT0@',
				'@OUTPUT1@'],
			install: true,
			install_dir: ''
)
lib_src += gen_trans_hdr_code
gen_deps += declare_dependency(sources: gen_trans_hdr_code)
