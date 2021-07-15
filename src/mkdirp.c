
//
// mkdirp.c
//
// Copyright (c) 2013 Stephen Mathieson
// MIT licensed
//

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "mem_pool.h"
#include "mkdirp.h"
#include "path-normalize.h"
#include "utils.h"

#ifdef _WIN32
#define PATH_SEPARATOR   '\\'
#else
#define PATH_SEPARATOR   '/'
#endif

extern struct mem_pool *g_mp;

/*
 * Recursively `mkdir(path, mode)`
 */

int
mkdirp(const char *path, mode_t mode) {
  char *pathname = NULL;
  char *parent = NULL;

  if (NULL == path) return -1;

  pathname = path_normalize(path);
  if (NULL == pathname) goto fail;

  parent = __strdup(pathname);
  if (NULL == parent) goto fail;

  char *p = parent + strlen(parent);
  while (PATH_SEPARATOR != *p && p != parent) {
    p--;
  }
  *p = '\0';

  // make parent dir
  if (p != parent && 0 != mkdirp(parent, mode)) goto fail;
  mp_free(g_mp, parent);

  // make this one if parent has been made
  #ifdef _WIN32
    // http://msdn.microsoft.com/en-us/library/2fkk4dzw.aspx
    int rc = mkdir(pathname);
  #else
    int rc = mkdir(pathname, mode);
  #endif

  mp_free(g_mp, pathname);

  return 0 == rc || EEXIST == errno
    ? 0
    : -1;

fail:
  mp_free(g_mp, pathname);
  mp_free(g_mp, parent);
  return -1;
}
