#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include "math64.h"
#include "strtox.h"
#include "compiler.h"

static inline int kstrtou32(const char *s, unsigned int base, u32 *res)
{
    return kstrtouint(s, base, res);
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

#if 0
int main(void) {
	char *str[] = {"freq=1300", "memsize1=60", "memsize2=180"};
	int freq = 0;
	int memsize1 = 0;
	int memsize2 = 0;

	char **p = str;
	parse_even_earlier(freq, "freq", *p);
	++p;
	parse_even_earlier(memsize1, "memsize1", *p);
	++p;
	parse_even_earlier(memsize2, "memsize2", *p);
	++p;

	printf("freq: %d, memsize1: %d, memsize2: %d\n", freq, memsize1, memsize2);

	return 0;
}
#endif
