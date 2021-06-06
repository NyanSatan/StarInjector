#include <image3.h>
#include <iboot.h>
#include <shsh.h>
#include <cmd.h>

image3_handle_t *image3_new(uint32_t ident) {
    image3_handle_t *result = (image3_handle_t *)malloc(sizeof(image3_handle_t));
    result->ident = ident;
    result->tags = NULL;

    return result;
}

void image3_free(image3_handle_t *handle, bool with_tag_data) {
    image3_tag_handle_t *tag = handle->tags;

    while (tag) {
        image3_tag_handle_t *next_tag = tag->next;

        if (with_tag_data)
            free(tag->data);

        free((void*)tag);
        
        tag = next_tag;
    }

    free(handle);
}

/*
 * Redo this
 */

void image3_add_tag(image3_handle_t *handle, image3_tag_handle_t *tag_handle) {
    image3_tag_handle_t **tag = &handle->tags;

    while (1) {
        if (!*tag) {
            *tag = tag_handle;
            return;
        }

        tag = &((*tag)->next);
    }
}

size_t image3_calculate(image3_handle_t *handle) {
    size_t result = sizeof(image3_header_t);

    image3_tag_handle_t *tag = handle->tags;

    while (tag) {
        result += sizeof(image3_tag_header_t) + tag->aligned_size;
        tag = tag->next;
    }

    return result;
}

void image3_generate(image3_handle_t *handle, void *dest) {
    image3_header_t *header = dest;
    header->magic = IMAGE3_MAGIC;
    header->ident = handle->ident;
    header->full_size = sizeof(image3_header_t); //also relative pointer
    header->data_size = 0;
    header->sig_check_size = 0;

    image3_tag_handle_t *tag = handle->tags;
    bool need_sig_check_area_incr = true;

    while (tag) {
        uint32_t tag_size = sizeof(image3_tag_header_t) + tag->aligned_size;

        image3_tag_header_t *new_tag = (image3_tag_header_t *)(dest + header->full_size);
        new_tag->ident = tag->ident;
        new_tag->full_size = tag_size;
        new_tag->data_size = tag->size;

        memmove(new_tag + 1, tag->data, tag->size);

        if (tag->size != tag->aligned_size)
            memset((uint8_t *)(new_tag + 1) + tag->size, 0x0, tag->aligned_size - tag->size);

        if (tag->ident == SHSH_TAG_IDENT)
            need_sig_check_area_incr = false;

        header->full_size += tag_size;
        header->data_size += tag_size;
        if (need_sig_check_area_incr)
            header->sig_check_size += tag_size;

        tag = tag->next;
    }
}

#define ALIGNMENT 4

image3_tag_handle_t *image3_tag_new(uint32_t ident, void *data, size_t size) {
    image3_tag_handle_t *result = (image3_tag_handle_t *)malloc(sizeof(image3_tag_handle_t));
    result->ident = ident;
    result->size = size;

    if (size % ALIGNMENT)
        result->aligned_size = size + ALIGNMENT - (size % ALIGNMENT); 
    else
        result->aligned_size = size;
    
    result->data = data;
    result->next = NULL;

    return result;
}

image3_tag_header_t *image3_get_tag_struct(image3_header_t *header, uint32_t tag_ident) {
    if (header->magic != IMAGE3_MAGIC) {
        printf("not an Image3\n");
        return NULL;
    }

    image3_tag_header_t *tag_header;

    for (size_t i = 0; i < header->data_size;) {
        tag_header = (image3_tag_header_t *)((uint8_t *)(header + 1) + i);

        if (tag_header->ident == tag_ident)
            return tag_header;

        i += tag_header->full_size;
    }

    return NULL;
}

#define SHSH_TAG_LENGTH_FULL 128

int do_image3_localize(int argc, struct cmd_arg *args) {
    void *addr;

    switch (argc) {
        case 1:
            addr = (void *)TARGET_LOADADDR;
            break;

        case 2:
            addr = (void *)args[1].u;
            break;

        default:
            printf("localize [address]\n");
            return ENOARG;
    }

    image3_tag_header_t *shsh_tag = image3_get_tag_struct(addr, SHSH_TAG_IDENT);
    if (!shsh_tag) {
        printf("failed to find SHSH tag in the requested image\n");
        return EFAILURE;
    }

    if (shsh_tag->data_size & 0xF) {
        printf("invalid SHSH tag data size\n");
        return EFAILURE;
    }

    if (shsh_tag->data_size != SHSH_TAG_LENGTH_FULL) {
        printf("weird SHSH tag data_size, continuing though\n");
    }

    if (shsh_key_action(kSHSHKeyEncrypt, &shsh_tag->data, &shsh_tag->data, shsh_tag->data_size) != 0) {
        printf("failed to encrypt with SHSH tag\n");
        return EFAILURE;
    }

    printf("done\n");

    return ESUCCESS;
}

MENU_COMMAND("localize", do_image3_localize, "encrypts SHSH in Image3");
