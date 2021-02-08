#ifndef DER_H_included
#define DER_H_included

#include <libc.h>

#define DER_SEQUENCE_HEADER_4_LEN 	0x6

void der_sequence_header(uint8_t *dest, size_t size);

#endif
