/**
****************************************************************************
 * @file startup.S
 * @brief U-Boot DDR 镜像的 Cortex-A7 ARM 状态启动入口。
 * @version 1.2
 * @date 2026-10-05
 * @author Codex
 * @history 1.1: 按仓库编码规范整理。
 * @history 1.2: 改用中文函数注释，移除版权字段，补充 Style 1.4 段落布局。
 * @history 1.3: 增加 Cortex-A7 异常向量表和 IRQ 入口。
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
.global _irq_entry

/* Cortex-A7 exception vector table; U-Boot enters the first instruction. */
.section .text.startup, "ax", %progbits
.global vector_table
vector_table:
    b _start
    b .Ldefault_exception
    b .Ldefault_exception
    b .Ldefault_exception
    b .Ldefault_exception
    b .Ldefault_exception
    b _irq_entry
    b .Ldefault_exception

.text

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

    /* Install this image's vectors and reserve a separate IRQ mode stack. */
    ldr r0, =vector_table
    mcr p15, 0, r0, c12, c0, 0
    dsb
    isb
    mrs r0, cpsr
    bic r1, r0, #0x1f
    orr r1, r1, #0x12
    msr cpsr_c, r1
    ldr sp, =_estack
    sub sp, sp, #0x100
    msr cpsr_c, r0

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

.Ldefault_exception:
    b .Ldefault_exception

/* IRQ mode entry preserves the interrupted registers before C dispatch. */
/**
****************************************************************************
 * @fn _irq_entry
 * @brief 保存 IRQ 中断现场，调用 GIC 分发函数并恢复现场。
 * @param 无；硬件进入 IRQ 模式时不通过 C 参数传递数据。
 * @return 通过异常返回指令返回被中断的程序位置。
****************************************************************************
 */
_irq_entry:
    sub lr, lr, #4
    stmfd sp!, {r0-r12, lr}
    bl HAL_GIC_HandleIrq
    ldmia sp!, {r0-r12, lr}
    subs pc, lr, #0

.size _start, . - _start

/* startup.S 文件结束。 */
