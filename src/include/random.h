#include <target/offsets.h>
#include <libc.h>

static int (*random_get_bytes)(uint8_t *buffer, size_t size) = (void *)TARGET_RANDOM_GET_BYTES;
