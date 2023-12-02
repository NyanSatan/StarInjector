#ifndef IBOOT_H_included
#define IBOOT_H_included

/*
 * Native iBoot structures & enumerations
 */

#include <libc.h>

struct cmd_arg {
    int32_t n;
    size_t u;
    size_t h;
    bool b;
    char *str;
};

/*
 * Native common iBoot functions
 */

#include <target/offsets.h>

static int 		(*printf)(const char *format, ...) = (void *)TARGET_PRINTF;
static int 		(*strcmp)(char const *cs, char const *ct) = (void *)TARGET_STRCMP;

static void 	(*memmove)(void *dest, void *src, size_t len) = (void *)TARGET_MEMMOVE;
static void 	(*memset)(void *dest, char sym, size_t len) = (void *)TARGET_MEMSET;

static void* 	(*malloc)(size_t size) = (void *)TARGET_MALLOC;
static void 	(*free)(void *ptr) = (void *)TARGET_FREE;

static size_t   (*env_get_uint)(const char *name, size_t default_val) = (void *)TARGET_ENV_GET_UINT;

#endif
