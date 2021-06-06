#include <target/offsets.h>
#include <bootpage.h>
#include <star.h>
#include <iboot.h>
#include <cmd.h>

bool did_inject = false, did_outsource = false;

int do_inject(int argc, struct cmd_arg *args) {
    if (did_outsource) {
        printf("injection is prohibited after outsourcing - reload iBoot and try again\n");
        return EFAILURE;
    }

    if (argc != 4) {
        printf("%s inject <ce> <source> <size>\n", new_cmd_string);
        return ENOARG;
    }

    uint16_t ce = args[1].h;
    void *source = (void *)args[2].u;
    size_t size = args[3].u;


    size_t page_count = size / TARGET_BOOT_PAGE_SIZE;

    if (size % TARGET_BOOT_PAGE_SIZE) {
        printf("advancing page count to fill the last one\n");
        page_count++;
    }

    if (page_count > TARGET_BOOT_PAGE_COUNT - TARGET_BOOT_PAGE_PTAB_COUNT) {
        printf("filesize is too big (max is 0x%x)\n", TARGET_BOOT_PAGE_COUNT - TARGET_BOOT_PAGE_PTAB_COUNT);
        return EFAILURE;
    }

    int ret = EFAILURE;

    printf("reading partition table\n");

    uint8_t *ptab = malloc(TARGET_BOOT_PAGE_SIZE);

    if (h2fmiPpnReadBootPage(ce, 0, ptab) != 0) {
        printf("failed to read partition table\n");
        goto out;
    }

    did_inject = true;

    printf("erasing block\n");

    if (h2fmiPpnEraseSingleBlock(ce, 0) != 0) {
		printf("failed to erase block\n");
		goto out;
	}

    printf("restoring partition table\n");

	if (h2fmiPpnWriteBootPage(ce, 0, ptab) != 0 || h2fmiPpnWriteBootPage(ce, 1, ptab) != 0) {
		printf("failed to restore partition table\n");
		goto out;
	}

    for (size_t i = 0; i < page_count; i++) {
        size_t page = i + TARGET_BOOT_PAGE_PTAB_COUNT;

        printf("writing page %d to CE %d\n", page, ce);

        if (h2fmiPpnWriteBootPage(ce, page, source + i * TARGET_BOOT_PAGE_SIZE) != 0) {
			printf("failed to write the page\n");
			goto out;
		}
    }

    printf("successfully written %d pages to CE %d\n", page_count, ce);
    ret = ESUCCESS;

out:
    free((void *)ptab);
    return ret;
}

MENU_COMMAND("inject", do_inject, "writes data to bootpage");


/*
 * TODO: move these 3 to a separate file
 */

void arm_clean_icache() {
    __asm__("MOV R0, #0x0");
    __asm__("MCR P15, 0, R0, C7, C5, 0");
    __asm__("DSB SY");
    __asm__("ISB SY");
}

void patch16(uint16_t *addr, uint16_t val) {
    printf("0x%04x -> 0x%04x @ 0x%x\n", *addr, val, addr);
    *(volatile uint16_t*)addr = val;
    arm_clean_icache();
}

void patch32(uint32_t *addr, uint32_t val) {
    printf("0x%08x -> 0x%08x @ 0x%x\n", *addr, val, addr);
    *(volatile uint32_t*)addr = val;
    arm_clean_icache();
}

int do_outsource(int argc, struct cmd_arg *args) {
    if (did_inject) {
        printf("outsourcing is prohibited after injection - reload iBoot and try again\n");
        return EFAILURE;
    }

    if (argc != 4) {
        printf("%s outsource <ce> <page> <source>\n", new_cmd_string);
        return ENOARG;
    }

    uint16_t ce = args[1].h;
    uint32_t page = args[2].u;
    void *source = (void *)args[3].u;

    int ret;

    did_outsource = true;

    printf("patching\n");

    uint32_t old1 = *(uint32_t *)TARGET_H2FMI_PATCH_ADDR_1;
    uint16_t old2 = *(uint16_t *)TARGET_H2FMI_PATCH_ADDR_2;

    patch32((uint32_t *)TARGET_H2FMI_PATCH_ADDR_1, TARGET_H2FMI_PATCH_VAL_1);
    patch16((uint16_t *)TARGET_H2FMI_PATCH_ADDR_2, TARGET_H2FMI_PATCH_VAL_2);

    ret = h2fmiPpnWriteBootPage(ce, page, source) != 0 ? EFAILURE : ESUCCESS;

    printf("unpatching\n");

    patch32((uint32_t *)TARGET_H2FMI_PATCH_ADDR_1, old1);
    patch16((uint16_t *)TARGET_H2FMI_PATCH_ADDR_2, old2);

    printf(ret == EFAILURE ? "failed to write the outsource page\n" : "success!\n");

    return ret;
}

MENU_COMMAND("outsource", do_outsource, "writes out-of-bounds data to bootpage");


int do_read(int argc, struct cmd_arg *args) {
    if (argc != 4) {
        printf("%s read <ce> <page> <dest>\n", new_cmd_string);
        return ENOARG;
    }

    uint16_t ce = args[1].h;
    uint32_t page = args[2].u;
    void *dest = (void *)args[3].u;

    if (page > TARGET_BOOT_PAGE_COUNT - 1) {
        printf("invalid page (max is 0x%x)\n", TARGET_BOOT_PAGE_COUNT - 1);
        return ENOARG;
    }

    printf("reading page %d from CE %d\n", page, ce);

    if (h2fmiPpnReadBootPage(ce, page, dest) != 0) {
        printf("failed to read bootpage\n");
        return EFAILURE;
    }

    return ESUCCESS;
}

MENU_COMMAND("read", do_read, "reads data from bootpage");

#if 0

static int32_t (*h2fmiPpnReadSinglePage)(uint16_t ce,
                                         uint32_t page,
                                         void *dest,
                                         void *meta_dest,
                                         void *max_corrected_bits_dest,
                                         void *sector_stats_dest,
                                         bool disable_whitening) = (void*)0xBFF09F44 + 1;


int do_single_read(int argc, struct cmd_arg *args) {
    if (argc != 5) {
        printf("%s sread <ce> <page> <dest> <meta dest>\n", new_cmd_string);
        return ENOARG;
    }

    uint16_t ce = args[1].h;
    uint32_t page = args[2].u;
    void *dest = (void *)args[3].u;
    void *meta_dest = (void *)args[4].u;

    if (page > TARGET_BOOT_PAGE_COUNT - 1) {
        printf("invalid page (max is 0x%x)\n", TARGET_BOOT_PAGE_COUNT - 1);
        return ENOARG;
    }

    printf("reading single page %d from CE %d\n", page, ce);

    int ret;

    if ((ret = h2fmiPpnReadSinglePage(ce, page, dest, meta_dest, NULL, NULL, true)) != 0) {
        printf("failed to read single page: 0x%x\n", ret);
        return EFAILURE;
    }

    return ESUCCESS;
}

MENU_COMMAND("sread", do_single_read, "reads data from single page");

#endif
