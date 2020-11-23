#ifndef _KSTRTOX_H_
#define _KSTRTOX_H_

#include <string.h>

#define KSTRTOX_OVERFLOW    (1U << 31)

#define parse_even_earlier(res, option, p)              \
do {                                    \
    unsigned int tmp;                \
                                    \
    if (strncmp(option, (char *)p, strlen(option)) == 0)        \
        tmp = kstrtou32((char *)p + strlen(option"="), 10, &res); \
} while (0)

static inline int kstrtou32(const char *s, unsigned int base, u32 *res);

int kstrtouint(const char *s, unsigned int base, unsigned int *res);

int kstrtoull(const char *s, unsigned int base, unsigned long long *res);

static int _kstrtoull(const char *s, unsigned int base, unsigned long long *res);

const char *_parse_integer_fixup_radix(const char *s, unsigned int *base);

static inline char _tolower(const char c);

static inline int isdigit(int ch);

static inline int isxdigit(int ch);

unsigned int _parse_integer(const char *s, unsigned int base, unsigned long long *p);

#endif
