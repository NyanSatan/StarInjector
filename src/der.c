#include <der.h>
#include <utils.h>

#define DER_SEQUENCE_MAGIC 		0x30
#define DER_SEQUENCE_HEADER_4	0x84

void der_sequence_header(uint8_t *dest, size_t size) {
	*dest++ = DER_SEQUENCE_MAGIC;
	*dest++ = DER_SEQUENCE_HEADER_4;
	*(uint32_t*)dest = rev(size);
}
