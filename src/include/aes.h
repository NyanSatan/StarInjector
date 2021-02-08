#ifndef AES_H_included
#define AES_H_included

#include <target/offsets.h>
#include <libc.h>

typedef enum {
	kAESTypeNormal = 0x0,
	kAESTypeGID  = 0x20000200,
	kAESTypeUID  = 0x20000201,
	kAESTypeSHSH = 0x100
} AESMode;

typedef enum {
	kAESEncrypt = 0x10,
	kAESDecrypt = 0x11
} AESOption;

static int (*aes_crypto_cmd)(AESOption option, void* input, void* output, size_t size,
							 AESMode mode, void* iv, void* key) = (void *)TARGET_AES_CRYPTO_CMD;

#endif
