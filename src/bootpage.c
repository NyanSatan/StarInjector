#include <target/offsets.h>
#include <bootpage.h>
#include <star.h>
#include <iboot.h>
#include <cmd.h>

int do_inject(int argc, struct cmd_arg *args) {
    if (argc != 4) {
        printf("%s inject <ce> <source> <size>\n", new_cmd_string);
        return ENOARG;
    }

    uint16_t ce = args[1].h;
    void *source = (void *)args[2].u;
    size_t size = args[3].u;

    size_t page_count = size / TARGET_BOOT_PAGE_SIZE;

    if (size % TARGET_BOOT_PAGE_SIZE) {
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
