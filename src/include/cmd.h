#ifndef CMD_H_included
#define CMD_H_included

#include <iboot.h>

/*
 * Command handler stuff
 */

typedef int (*cmd_func)(int argc, struct cmd_arg *args);

struct star_cmd_menu_item {
	const char *cmd;
	cmd_func func;
	const char *help;
};

#define MENU_COMMAND(_cmd, _func, _help) \
struct star_cmd_menu_item _func##_struct __attribute__((section("__TEXT,__commands"))) = { \
    .cmd = _cmd, \
    .func = _func, \
    .help = _help \
};

int cmd_handler(int argc, struct cmd_arg *args);

/*
 * Errors
 */

#define ESUCCESS  0
#define EFAILURE -1
#define ENOARG   -2

#endif
