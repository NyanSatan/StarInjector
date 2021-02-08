#include <cmd.h>
#include <iboot.h>
#include <libc.h>
#include <star.h>

bool inited = false;

void after_early_init() {
    inited = true;

    printf("✨🌙 Say hello to Star Injector!\n");
    printf("version: %s, target: %s\n", build_tag_string, build_target_string);
    printf("relocated successfully, from now on call me with \"%s\"\n", new_cmd_string);
    printf(command_separator);
}

int _main(int argc, struct cmd_arg *args) {
    if (!inited) {
        after_early_init();
        return 0;
    } else {
        return cmd_handler(argc, args);
    }
}
