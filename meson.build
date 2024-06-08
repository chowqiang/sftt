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

project('SFTT', 'c',
	version: run_command(find_program('cat'),
		files('VERSION'), check: true).stdout().strip(),
	license: 'BSD',
	default_options: [
		'buildtype=release',
		#'c_std=c11',
		'default_library=static',
		'warning_level=2',
	],
	meson_version: '>= 0.53.2'
)

sftt_inc = include_directories('include', 'config', 'xdr')
sftt_ldflags = ['-pthread', '-lcurses', '-lsqlite3', '-lreadline']

# deps for generated files
gen_deps = []

# collect source files for lib
lib_src = []
sftt_lib = []

# deps for lib
lib_deps = []

# build config
subdir('config')

# build xdr
subdir('xdr')

# build lib
subdir('src')

# build server app
subdir('server')

# build client app
subdir('client')

# build tools
subdir('tools')

# build tests
#subdir('tests')
