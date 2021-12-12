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

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include "debug.h"

int default_debug_level = DEBUG_WARN;

pthread_key_t dbug_key;

void set_client_debug_level(int verbose)
{
	if (verbose == 1) {
		default_debug_level = DEBUG_INFO;
	} else if (verbose == 2) {
		default_debug_level = DEBUG_DEBUG;
	} else if (verbose > 2) {
		default_debug_level = DEBUG_DEBUG;
	} else {
		default_debug_level = DEBUG_WARN;
	}
}

void set_server_debug_level(int verbose)
{
	if (verbose == 1) {
		default_debug_level = DEBUG_DEBUG;
	} else if (verbose > 1) {
		default_debug_level = DEBUG_DEBUG;
	} else {
		default_debug_level = DEBUG_INFO;
	}
}

void _db_enter_(const char *_func_, const char *_file_,
		int _line_, struct _db_stack_frame_ *_stack_frame_)
{
	struct _db_code_state_ *cs;

	cs = pthread_getspecific(dbug_key);
	assert(cs != NULL);

	_stack_frame_->func = _func_;
	_stack_frame_->file = _file_;
	_stack_frame_->line = _line_;

	_stack_frame_->level = ++cs->level;
	_stack_frame_->prev = cs->framep;
	cs->framep = _stack_frame_;
}

void _db_return_(struct _db_stack_frame_ *_stack_frame_)
{
	struct _db_code_state_ *cs;

	cs = pthread_getspecific(dbug_key);
	assert(cs != NULL);

	assert(cs->framep != NULL);

	cs->framep = cs->framep->prev;
}

void _db_dump_(void)
{
	struct _db_stack_frame_ *_stack_frame_;
	struct _db_code_state_ *cs;

	cs = pthread_getspecific(dbug_key);
	assert(cs != NULL);

	_stack_frame_ = cs->framep;
	while (_stack_frame_) {
		printf("#%d %s() at %s:%d\n", _stack_frame_->level,
				_stack_frame_->func, _stack_frame_->file,
				_stack_frame_->line);
		_stack_frame_ = _stack_frame_->prev;
	}
}

static void __attribute__((constructor)) dbug_key_init(void)
{
	struct _db_code_state_ *cs;

	cs = (struct _db_code_state_ *)malloc(sizeof(struct _db_code_state_));
	cs->framep = NULL;
	cs->level = 0;

	pthread_key_create(&dbug_key, NULL);
	pthread_setspecific(dbug_key, cs);
}
