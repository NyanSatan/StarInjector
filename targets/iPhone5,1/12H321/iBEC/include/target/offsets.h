#ifndef OFFSETS_H_included
#define OFFSETS_H_included

/*
 * Target's memory map
 */

#define TARGET_BASEADDR         0xBFF00000
#define TARGET_SRAMADDR         0x10000000
#define TARGET_LOADADDR         0x80000000
#define TARGET_JUMPADDR         0x84000000

#define TARGET_CMD_PATCH        (TARGET_BASEADDR + 0x4318C)

/*
 * Target's common C functions
 */

#define TARGET_PRINTF           (TARGET_BASEADDR + (0x33B1C + 1))
#define TARGET_STRCMP           (TARGET_BASEADDR + (0x34AA8 + 1))
#define TARGET_MEMMOVE          (TARGET_BASEADDR + 0x345B8)
#define TARGET_MEMSET           (TARGET_BASEADDR + 0x348D4)

/*
 * Target's heap functions
 */

#define TARGET_MALLOC           (TARGET_BASEADDR + (0x19B70 + 1))
#define TARGET_FREE             (TARGET_BASEADDR + (0x19C24 + 1))

/*
 * Target's H2FMI functions and stuff
 */

#define TARGET_H2FMI_ERASE_SINGLE_BLOCK (TARGET_BASEADDR + (0x9B98 + 1))
#define TARGET_H2FMI_READ_BOOT_PAGE     (TARGET_BASEADDR + (0x9990 + 1))
#define TARGET_H2FMI_WRITE_BOOT_PAGE    (TARGET_BASEADDR + (0x9AA0 + 1))

#define TARGET_BOOT_PAGE_SIZE       0x600
#define TARGET_BOOT_PAGE_COUNT      0x100
#define TARGET_BOOT_PAGE_PTAB_COUNT 0x2

#define TARGET_H2FMI_PATCH_ADDR_1       (TARGET_BASEADDR + 0x7E7A)
#define TARGET_H2FMI_PATCH_VAL_1        0x4000F240
#define TARGET_H2FMI_PATCH_ADDR_2       (TARGET_BASEADDR + 0x7EDE)
#define TARGET_H2FMI_PATCH_VAL_2        0x2D02

/*
 * Target's misc functions
 */

#define TARGET_AES_CRYPTO_CMD       (TARGET_BASEADDR + (0x223E4 + 1))
#define TARGET_RANDOM_GET_BYTES     (TARGET_BASEADDR + (0x1DE30 + 1))

#endif
