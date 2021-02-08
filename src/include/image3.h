#ifndef UTILS_H_included
#define UTILS_H_included

#include <libc.h>

/*
 * Image3 structs
 */

typedef struct {
    uint32_t magic;
#define IMAGE3_MAGIC 'Img3'
    uint32_t full_size;
    uint32_t data_size;
    uint32_t sig_check_size;
    uint32_t ident;
#define LLB_IDENT   'illb'
#define IBSS_IDENT  'ibss'
    uint8_t data[];
} image3_header_t;

typedef struct {
    uint32_t ident;
#define DATA_TAG_IDENT 'DATA'
#define SHSH_TAG_IDENT 'SHSH'
#define CERT_TAG_IDENT 'CERT'
    uint32_t full_size;
    uint32_t data_size;
    uint8_t data[];
} image3_tag_header_t;

/*
 * Image3 creation
 */

typedef struct image3_tag_handle {
    struct image3_tag_handle *next;
    uint32_t ident;
    size_t size;
    size_t aligned_size;
    void *data;
} image3_tag_handle_t;

typedef struct {
    uint32_t ident;
    image3_tag_handle_t *tags;
} image3_handle_t;

image3_handle_t     *image3_new(uint32_t ident);
void                 image3_free(image3_handle_t *handle, bool with_tag_data);
void                 image3_add_tag(image3_handle_t *handle, image3_tag_handle_t *tag_handle);
size_t               image3_calculate(image3_handle_t *handle);
void                 image3_generate(image3_handle_t *handle, void *dest);

image3_tag_handle_t *image3_tag_new(uint32_t ident, void *data, size_t size);

/*
 * Image3 parsing
 */

image3_tag_header_t *image3_get_tag_struct(image3_header_t *header, uint32_t tag_ident);

#endif
