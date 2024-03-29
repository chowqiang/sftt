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

#ifndef _SERIALIZE_HANDLE_H_
#define _SERIALIZE_HANDLE_H_

struct serialize_handler {
	int packet_type;
	bool (*serialize)(void *obj, unsigned char **buf, int *len,
			enum free_mode *mode);
	bool (*deserialize)(unsigned char *buf, int len, void **obj,
			enum free_mode *mode);
};

#endif
