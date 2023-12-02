/*
 * Relocating our payload from load address
 * to somewhere else (usually jump address)
 */

#include <target/offsets.h>

.arm
.globl start
.align 4

start:
    /*
     * Getting where we’re running
     */

	MOV R2, PC
	SUB R2, #0x8

    /*
     * Getting where we must be running
     */

    LDR R3, =TARGET_RELOCATE_ADDR

relocate:
    STMDB SP!, {R4-R7}
    ADD R7, SP, #0xC

    /*
     * Relocating payload
     */

    LDR R5, L__payload_end
    SUB R5, R3
    MOV R4, #0x0

    relocate_loop:
        LDR R6, [R2, R4]
        STR R6, [R3, R4]
        ADD R4, #0x4
        CMP R5, R4 
        BHI relocate_loop

zero_bss:
    LDR R4, L__bss_end
    LDR R5, L__payload_end
    MOV R6, #0x0

    zero_bss_loop:
        STR R6, [R5], #0x4
        CMP R5, R4
        BHI zero_bss_loop

patch_cmd:
    LDR R4, =TARGET_CMD_PATCH

    LDR R2, _new_cmd_string
    STR R2, [R4]

    LDR R2, L__main
    STR R2, [R4, #0x4]

    LDMIA SP!, {R4-R7}

clear_icache:
    /*
     * Clearing instruction cache
     */

    MOV R3, #0x0
    MCR P15, 0, R3, C7, C5, 0
    DSB	SY
    ISB SY

main_trampoline:
    /*
	 * Jumping to C code
	 */

    BX R2

L__new_cmd:
    .asciz "starinjector"


/*
 * Build information
 */

.org 0xA0
L__build_banner_string:
    .ascii PROJ_NAME
    .ascii ", Copyright 2020-2023, john (@nyan_satan)"
.org 0xE0
L__build_target_string:
    .ascii TARGET
.org 0x120
L__build_tag_string:
	.ascii "BUILD_TAG_STRING"
.org 0x180

/*
 * Consts used by entry code only
 */

L__bss_end:
    .long section$end$__DATA$__zerofill

L__payload_end:
	.long section$start$__DATA$__zerofill

L__main:
	.long __main

/*
 * Consts used globally and after relocation
 */

	.globl _build_banner_string
_build_banner_string:
	.long L__build_banner_string

	.globl _build_target_string
_build_target_string:
	.long L__build_target_string

	.globl _build_tag_string
_build_tag_string:
	.long L__build_tag_string

	.globl _new_cmd_string
_new_cmd_string:
	.long L__new_cmd

