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

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "option.h"

/**
 * skip_spaces - Removes leading whitespace from @str.
 * @str: The string to be stripped.
 *
 * Returns a pointer to the first non-whitespace character in @str.
 **/
char *skip_spaces(const char *str)
{
    while (isspace(*str))
        ++str;
    return (char *)str;
}

/**
 * Parse a string to get a param value pair.
 * You can use " around spaces, but can't escape ".
 * Hyphens and underscores equivalent in parameter names.
 **/
char *next_arg(char *args, char **param, char **val)
{
    unsigned int i, equals = 0;
    int in_quote = 0, quoted = 0;
    char *next;

    if (*args == '"') {
        args++;
        in_quote = 1;
        quoted = 1;
    }

    for (i = 0; args[i]; i++) {
        if (isspace(args[i]) && !in_quote)
            break;
        if (equals == 0) {
            if (args[i] == '=')
                equals = i;
        }
        if (args[i] == '"')
            in_quote = !in_quote;
    }

    *param = args;
	printf("1: %p\n", *param);
    if (!equals)
        *val = NULL;
    else {
        args[equals] = '\0';
        *val = args + equals + 1;

        /* Don't include quotes in value. */
        if (**val == '"') {
            (*val)++;
            if (args[i-1] == '"')
                args[i-1] = '\0';
        }
    }
    if (quoted && args[i-1] == '"')
        args[i-1] = '\0';

    if (args[i]) {
        args[i] = '\0';
        next = args + i + 1;
    } else
        next = args + i;

    /* Chew up trailing spaces. */
    return skip_spaces(next);
}

const struct sftt_option *lookup_opt(int argc, char **argv, char **optarg,
	int *optind, const struct sftt_option *sftt_opts)
{
	*optarg = NULL;
	if (*optind >= argc) {
		return NULL;
	}

	const struct sftt_option *opt = sftt_opts;
	for (;;) {
		if (!opt->name) {
			return NULL;
		}
		if (!strcmp(opt->name, argv[*optind])) {
			break;
		}
		++opt;
	}

	(*optind)++;
	if (opt->flags & HAS_ARG) {
		if (*optind < argc) {
			*optarg = argv[*optind];
			(*optind)++;
		}
	} else if (opt->flags & OPT_ARG) {
		if (*optind < argc) {
			*optarg = argv[*optind];
		}
	}

	return opt;
}

int sftt_option_test(void)
{
	char *str = "a=12 b=12";
	char *param = NULL, *value = NULL;
	
	char data[128];
	strcpy(data, str);
	next_arg(data, &param, &value);
	printf("param: %s, value: %s\n", param, value);

	return 0;
}
