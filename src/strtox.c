/*
 * Copyright (C) copy from linux kernel.
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

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include "math64.h"
#include "strtox.h"
#include "compiler.h"

static int _kstrtoull(const char *s, unsigned int base, unsigned long long *res);
static inline char _tolower(const char c);
static inline int isxdigit(int ch);

int kstrtou32(const char *s, unsigned int base, u32 *res)
{
    return kstrtouint(s, base, res);
}

int parse_even_earlier(u32 *res, const char *option, char *p)
{
    int tmp = -1;

    if (strncmp(option, (char *)p, strlen(option)) == 0)
        tmp = kstrtou32((char *)p + strlen(option) + 1, 10, res);

    return tmp;
}

/**
 * kstrtouint - convert a string to an unsigned int
 * @s: The start of the string. The string must be null-terminated, and may also
 *  include a single newline before its terminating null. The first character
 *  may also be a plus sign, but not a minus sign.
 * @base: The number base to use. The maximum supported base is 16. If base is
 *  given as 0, then the base of the string is automatically detected with the
 *  conventional semantics - If it begins with 0x the number will be parsed as a
 *  hexadecimal (case insensitive), if it otherwise begins with 0, it will be
 *  parsed as an octal number. Otherwise it will be parsed as a decimal.
 * @res: Where to write the result of the conversion on success.
 *
 * Returns 0 on success, -ERANGE on overflow and -EINVAL on parsing error.
 * Used as a replacement for the obsolete simple_strtoull. Return code must
 * be checked.
 */
int kstrtouint(const char *s, unsigned int base, unsigned int *res)
{
    unsigned long long tmp;
    int rv;

    rv = kstrtoull(s, base, &tmp);
    if (rv < 0)
        return rv;
    if (tmp != (unsigned long long)(unsigned int)tmp)
        return -ERANGE;
    *res = tmp;
    return 0;
}

/**
 * kstrtoull - convert a string to an unsigned long long
 * @s: The start of the string. The string must be null-terminated, and may also
 *  include a single newline before its terminating null. The first character
 *  may also be a plus sign, but not a minus sign.
 * @base: The number base to use. The maximum supported base is 16. If base is
 *  given as 0, then the base of the string is automatically detected with the
 *  conventional semantics - If it begins with 0x the number will be parsed as a
 *  hexadecimal (case insensitive), if it otherwise begins with 0, it will be
 *  parsed as an octal number. Otherwise it will be parsed as a decimal.
 * @res: Where to write the result of the conversion on success.
 *
 * Returns 0 on success, -ERANGE on overflow and -EINVAL on parsing error.
 * Used as a replacement for the obsolete simple_strtoull. Return code must
 * be checked.
 */
int kstrtoull(const char *s, unsigned int base, unsigned long long *res)
{
    if (s[0] == '+')
        s++;
    return _kstrtoull(s, base, res);
}

static int _kstrtoull(const char *s, unsigned int base, unsigned long long *res)
{
    unsigned long long _res;
    unsigned int rv;

    s = _parse_integer_fixup_radix(s, &base);
    rv = _parse_integer(s, base, &_res);
    if (rv & KSTRTOX_OVERFLOW)
        return -ERANGE;
    if (rv == 0)
        return -EINVAL;
    s += rv;
    if (*s == '\n')
        s++;
    if (*s)
        return -EINVAL;
    *res = _res;
    return 0;
}

const char *_parse_integer_fixup_radix(const char *s, unsigned int *base)
{
    if (*base == 0) {
        if (s[0] == '0') {
            if (_tolower(s[1]) == 'x' && isxdigit(s[2]))
                *base = 16;
            else
                *base = 8;
        } else
            *base = 10;
    }
    if (*base == 16 && s[0] == '0' && _tolower(s[1]) == 'x')
        s += 2;
    return s;
}

/*
 * Fast implementation of tolower() for internal usage. Do not use in your
 * code.
 */
static inline char _tolower(const char c)
{
    return c | 0x20;
}

static inline int isdigit(int ch)
{
    return (ch >= '0') && (ch <= '9');
}

static inline int isxdigit(int ch)
{
    if (isdigit(ch))
        return true;

    if ((ch >= 'a') && (ch <= 'f'))
        return true;

    return (ch >= 'A') && (ch <= 'F');
}

unsigned int _parse_integer(const char *s, unsigned int base, unsigned long long *p)
{
    unsigned long long res;
    unsigned int rv;

    res = 0;
    rv = 0;
    while (1) {
        unsigned int c = *s;
        unsigned int lc = c | 0x20; /* don't tolower() this line */
        unsigned int val;

        if ('0' <= c && c <= '9')
            val = c - '0';
        else if ('a' <= lc && lc <= 'f')
            val = lc - 'a' + 10;
        else
            break;

        if (val >= base)
            break;
        /*
         * Check for overflow only if we are within range of
         * it in the max base we support (16)
         */
        if (unlikely(res & (~0ull << 60))) {
            if (res > div_u64(ULLONG_MAX - val, base))
                rv |= KSTRTOX_OVERFLOW;
        }
        res = res * base + val;
        rv++;
        s++;
    }
    *p = res;
    return rv;
}
