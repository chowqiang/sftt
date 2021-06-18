/*
 * Copyright (C)  2020-2021 Min Zhou <zhoumin@bupt.cn>, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _STATE_H_
#define _STATE_H_

enum cmd_args_state {
	INIT,
	RECEIVE_SINGLE_QUOTE,
	AMONG_SINGLE_QUOTE,
	RECEIVE_DOUBLE_QUOTE,
	AMONG_DOUBLE_QUOTE,
	SUBSTR_END,
	FINAL,	
};

#endif
