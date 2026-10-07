.syntax unified
.cpu cortex-a7
.arm

/* U-Boot 'go 0xc0008000' enters here in ARM state. */
.section .text.startup, "ax", %progbits
.global _start
.type _start, %function
/*
 * @brief 设置 A7 栈，初始化 data/bss 段，然后进入 C 程序 main。
 * @fn _start
 * @param 无 本函数无参数。
 * @return 无；启动后进入常驻运行的 main 函数。
 */
_start:
    ldr sp, =_estack

    /* Copy initialized data from the image and clear the BSS. */
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata
1:
    cmp r1, r2
    bhs 2f
    ldr r3, [r0], #4
    str r3, [r1], #4
    b 1b
2:
    ldr r1, =_sbss
    ldr r2, =_ebss
    mov r3, #0
3:
    cmp r1, r2
    bhs 4f
    str r3, [r1], #4
    b 3b
4:
    bl main
5:
    b 5b
.size _start, . - _start
