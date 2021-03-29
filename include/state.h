#ifndef _STATE_H_
#define _STATE_H_

typedef enum cmd_args_state {
	INIT,
	RECEIVE_SINGLE_QUOTE,
	AMONG_SINGLE_QUOTE,
	RECEIVE_DOUBLE_QUOTE,
	AMONG_DOUBLE_QUOTE,
	SUBSTR_END,
	FINAL,	
} cmd_args_state;

#endif
