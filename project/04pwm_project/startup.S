/**
****************************************************************************
 * @file startup.S
 * @brief U-Boot DDR 镜像的 Cortex-A7 ARM 状态启动入口。
 * @version 1.2
 * @date 2026-10-05
 * @author Codex
 * @history 1.1: 按仓库编码规范整理。
 * @history 1.2: 改用中文函数注释，移除版权字段，补充 Style 1.4 段落布局。
 * @attention Cortex-A7 裸机代码，规范偏离项见 Agent.md。
****************************************************************************
 */

.syntax unified
.cpu cortex-a7
.arm

/* U-Boot 'go 0xc0008000' must enter this image in ARM state. */
.section .text.startup, "ax", %progbits
.global _start
.type _start, %function

/**
****************************************************************************
 * @fn _start
 * @brief 设置 A7 栈，初始化 data/bss 段，然后调用 main。
 * @param 无。
 * @return 不返回；main 返回后进入停机循环，会修改 r0～r3 和 sp 寄存器。
****************************************************************************
 */
_start:
    ldr sp, =_estack

    /* Linker symbols are aligned to four bytes for word-sized copies. */
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata
.Lcopy_data:
    cmp r1, r2
    bhs .Lclear_setup
    ldr r3, [r0], #4
    str r3, [r1], #4
    b .Lcopy_data

.Lclear_setup:
    ldr r1, =_sbss
    ldr r2, =_ebss
    mov r3, #0
.Lclear_bss:
    cmp r1, r2
    bhs .Lcall_main
    str r3, [r1], #4
    b .Lclear_bss

.Lcall_main:
    bl main
.Lhalt:
    b .Lhalt

.size _start, . - _start

/* startup.S 文件结束。 */
