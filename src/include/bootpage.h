#ifndef BOOTPAGE_H_included
#define BOOTPAGE_H_included

#include <target/offsets.h>
#include <libc.h>

static int32_t (*h2fmiPpnReadBootPage)(uint16_t ce,
                                    uint32_t page,
                                    void* dest) = (void*)TARGET_H2FMI_READ_BOOT_PAGE;

static int32_t (*h2fmiPpnWriteBootPage)(uint16_t ce,
                                        uint32_t page,
                                        void* source) = (void*)TARGET_H2FMI_WRITE_BOOT_PAGE;

static int32_t (*h2fmiPpnEraseSingleBlock)(uint16_t ce,
                               		       uint32_t block) = (void*)TARGET_H2FMI_ERASE_SINGLE_BLOCK;

#endif
