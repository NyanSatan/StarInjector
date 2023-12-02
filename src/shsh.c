#include <shsh.h>
#include <aes.h>
#include <cmd.h>

static const uint8_t derivedSeed[] = {0xdb, 0x1f, 0x5b, 0x33, 0x60, 0x6c, 0x5f, 0x1c,
                                      0x19, 0x34, 0xaa, 0x66, 0x58, 0x9c, 0x06, 0x61};

uint8_t shsh_key[16];
bool _shsh_key_is_computed = false;

int shsh_key_compute() {
	if (_shsh_key_is_computed) {
		return 0;
	}

	printf("computing SHSH key\n");

	if (aes_crypto_cmd(kAESEncrypt, (void *)&derivedSeed, &shsh_key, sizeof(shsh_key), kAESTypeSHSH, 0, 0) != 0) {
		printf("failed to compute SHSH key\n");
        return -1;
    }

    _shsh_key_is_computed = true;

    return 0;
}

bool shsh_key_is_computed() {
	return _shsh_key_is_computed;
}

const void *shsh_key_get() {
	if (!_shsh_key_is_computed) {
		if (shsh_key_compute() != 0) {
			return NULL;
		}
	}

	return (const void *)&shsh_key;
}

int shsh_key_action(shsh_key_action_t action, void *in, void *out, size_t len) {
	if (!_shsh_key_is_computed) {
		if (shsh_key_compute() != 0) {
			return -1;
		}
	}

	if (aes_crypto_cmd(
		action == kSHSHKeyDecrypt ? kAESDecrypt : kAESEncrypt,
		in,
		out,
		len,
		kAESTypeNormal,
		(void *)&shsh_key,
		0
	    ) != 0) {
        return -1;
    }

    return 0;
}

int do_shsh_key(int argc, struct cmd_arg *args) {
	if (!_shsh_key_is_computed) {
		if (shsh_key_compute() != 0) {
			return EFAILURE;
		}
	}

	for (size_t i = 0; i < sizeof(shsh_key); i++) {
		printf("%02X", shsh_key[i]);
	}

	printf("\n");

    return ESUCCESS;
}

MENU_COMMAND("shsh", do_shsh_key, "print SHSH key");
