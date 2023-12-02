#ifndef OFFSETS_H_included
#define OFFSETS_H_included

/*
 * Target's memory map
 */

#define TARGET_BASEADDR         0xBFF00000
#define TARGET_SRAMADDR         0x10000000
#define TARGET_LOADADDR         0x80000000
#define TARGET_JUMPADDR         0x84000000

#define TARGET_CMD_PATCH        (TARGET_BASEADDR + 0x4143C)

/*
 * Target's common C functions
 */

#define TARGET_PRINTF           (TARGET_BASEADDR + (0x32EC0 + 1))
#define TARGET_STRCMP           (TARGET_BASEADDR + (0x33E80 + 1))
#define TARGET_MEMMOVE          (TARGET_BASEADDR + 0x339A4)
#define TARGET_MEMSET           (TARGET_BASEADDR + 0x33CC0)

/*
 * Target's heap functions
 */

#define TARGET_MALLOC           (TARGET_BASEADDR + (0x1A628 + 1))
#define TARGET_FREE             (TARGET_BASEADDR + (0x1A6D8 + 1))

/*
 * Target's H2FMI functions and stuff
 */

#define TARGET_H2FMI_ERASE_SINGLE_BLOCK (TARGET_BASEADDR + (0xA2C4 + 1))
#define TARGET_H2FMI_READ_BOOT_PAGE     (TARGET_BASEADDR + (0xA0C4 + 1))
#define TARGET_H2FMI_WRITE_BOOT_PAGE    (TARGET_BASEADDR + (0xA1CC + 1))

#define TARGET_BOOT_PAGE_SIZE       0x600
#define TARGET_BOOT_PAGE_COUNT      0x100
#define TARGET_BOOT_PAGE_PTAB_COUNT 0x2

#define TARGET_H2FMI_PATCH_ADDR_1       (TARGET_BASEADDR + 0x8448)
#define TARGET_H2FMI_PATCH_VAL_1        0x4100F240
#define TARGET_H2FMI_PATCH_ADDR_2       (TARGET_BASEADDR + 0x84BA)
#define TARGET_H2FMI_PATCH_VAL_2        0x2802

/*
 * Target's misc functions
 */

#define TARGET_AES_CRYPTO_CMD       (TARGET_BASEADDR + (0x22900 + 1))
#define TARGET_RANDOM_GET_BYTES     (TARGET_BASEADDR + (0x1E174 + 1))
#define TARGET_ENV_GET_UINT         (TARGET_BASEADDR + (0x18C94 + 1))


/*
 * Target's framebuffer stuff
 */

#define TARGET_FRAMEBUFFER_HEIGHT  (1136)
#define TARGET_FRAMEBUFFER_WIDTH   (640)

#endif
