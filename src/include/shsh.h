#ifndef SHSH_H_included
#define SHSH_H_included

#include <libc.h>

typedef enum {
    kSHSHKeyEncrypt = 0,
    kSHSHKeyDecrypt
} shsh_key_action_t;

int         shsh_key_compute();
bool        shsh_key_is_computed();
const void *shsh_key_get();
int         shsh_key_action(shsh_key_action_t action, void *in, void *out, size_t len);

#endif
