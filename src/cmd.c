#include <cmd.h>
#include <iboot.h>
#include <star.h>
#include <utils.h>

extern const void commands;
extern const void ecommands;

static const struct star_cmd_menu_item *gCommands = &commands;
static const void *gCommandsEnd = &ecommands;

struct star_cmd_menu_item *cmd_iterator(struct star_cmd_menu_item *iterator);

int do_hexdump(int argc, struct cmd_arg *args) {
    void *addr;
    size_t len;

#define DEFAULT_LENGTH 0x100

    switch (argc) {
        case 1:
            addr = (void *)TARGET_LOADADDR;
            len = DEFAULT_LENGTH;
            break;

        case 2:
            addr = (void *)args[1].u;
            len = DEFAULT_LENGTH;
            break;

        case 3:
            addr = (void *)args[1].u;
            len = args[2].u;
            break;

        default:
            printf("hexdump [address] [length]\n");
            return ENOARG;
    }
    
    hexdump(addr, len);
    return ESUCCESS;
}

MENU_COMMAND("hexdump", do_hexdump, "memory hexdump");


int do_help(int argc, struct cmd_arg *args) {
    printf("Star Injector command list:\n");

    struct star_cmd_menu_item *cmd = NULL;
    while ((cmd = cmd_iterator(cmd))) {
        if (cmd->help) {
            printf("        %-16.16s %s\n", cmd->cmd, cmd->help);
        }
    }

    return ESUCCESS;
}

MENU_COMMAND("help", do_help, NULL);


struct star_cmd_menu_item *cmd_iterator(struct star_cmd_menu_item *iterator) {
    if (iterator == NULL) {
        iterator = (struct star_cmd_menu_item *)gCommands;
    } else {
        iterator++;
    }

    if (iterator == gCommandsEnd) {
        iterator = NULL;
    }

    return iterator;
}

int cmd_handler(int argc, struct cmd_arg *args) {
    int ret;

    if (argc > 1) {
        struct star_cmd_menu_item *cmd = NULL;

        while ((cmd = cmd_iterator(cmd))) {
            if (strcmp(args[1].str, cmd->cmd) == 0) {
                ret = cmd->func(--argc, &args[1]);
                goto out;
            } 
        }

        printf("unrecognized command: %s\n", args[1].str);
        ret = ENOARG;
    } else {
        printf("already initialized, but no arg supplied. Try \"%s help\" for help\n", new_cmd_string);
        ret = ENOARG;
    }

out:
    printf(command_separator);
    return ret;
}
