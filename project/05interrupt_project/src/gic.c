/**
****************************************************************************
 * @file gic.c
 * @brief Cortex-A7 GIC-400 外部中断初始化和 IRQ 分发。
 * @version 1.0
 * @date 2026-10-05
 * @author Codex
 * @history 1.0: 完成 GIC SPI 23 初始化和 IRQ 分发。
 * @attention Cortex-A7 裸机代码，规范偏离项见 Agent.md。
****************************************************************************
 */

/* Includes ---------------------------------------------------------------------------------------------*/
#include "exti.h"
#include "gic.h"

/* Private macro ----------------------------------------------------------------------------------------*/
#define GICD_BASE_ADDRESS 0xA0021000U
#define GICC_BASE_ADDRESS 0xA0020000U
#define GICD_CTLR (*(volatile uint32_t *)(GICD_BASE_ADDRESS + 0x000U))
#define GICD_ISENABLER0 (*(volatile uint32_t *)(GICD_BASE_ADDRESS + 0x100U))
#define GICD_ICFGR1 (*(volatile uint32_t *)(GICD_BASE_ADDRESS + 0xC04U))
#define GICD_IPRIORITY23 (*(volatile uint8_t *)(GICD_BASE_ADDRESS + 0x417U))
#define GICD_ITARGETSR23 (*(volatile uint8_t *)(GICD_BASE_ADDRESS + 0x817U))
#define GICC_CTLR (*(volatile uint32_t *)(GICC_BASE_ADDRESS + 0x000U))
#define GICC_PMR (*(volatile uint32_t *)(GICC_BASE_ADDRESS + 0x004U))
#define GICC_IAR (*(volatile uint32_t *)(GICC_BASE_ADDRESS + 0x00CU))
#define GICC_EOIR (*(volatile uint32_t *)(GICC_BASE_ADDRESS + 0x010U))
#define GICD_IRQ_BIT (1U << GIC_EXTI9_IRQ)
#define GICD_IRQ_CONFIG_MASK (3U << 14U)

/* Private enum -----------------------------------------------------------------------------------------*/

/* Private typedef --------------------------------------------------------------------------------------*/

/* Exported variables -----------------------------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------------------------------*/

/* Private function declaration -------------------------------------------------------------------------*/

/* Exported function declaration ------------------------------------------------------------------------*/
/* 本模块接口已在 gic.h 中声明。 */

/* Exported functions、 prototypes-------------------------------------------------------------------------*/
/**
****************************************************************************
 * @fn HAL_GIC_Init
 * @brief 初始化 GIC Distributor 和 CPU interface，并使能 EXTI9 IRQ。
 * @param 无。
 * @return 无。
****************************************************************************
 */
void HAL_GIC_Init(void)
{
    GICD_CTLR = 0U;
    /* EXTI5..9 is a level-sensitive shared interrupt at the GIC input. */
    GICD_ICFGR1 &= ~GICD_IRQ_CONFIG_MASK;
    GICD_IPRIORITY23 = 0xA0U;
    GICD_ITARGETSR23 = 0x01U;
    GICD_ISENABLER0 = GICD_IRQ_BIT;
    GICD_CTLR = 1U;

    GICC_CTLR = 0U;
    GICC_PMR = 0xFFU;
    GICC_CTLR = 1U;
}

/**
****************************************************************************
 * @fn HAL_GIC_HandleIrq
 * @brief 读取 GIC 应答号，分发 EXTI9 中断并完成应答。
 * @param 无。
 * @return 无。
****************************************************************************
 */
void HAL_GIC_HandleIrq(void)
{
    uint32_t interrupt_ack = GICC_IAR;
    uint32_t irq_id = interrupt_ack & 0x3FFU;

    if (irq_id != GIC_SPURIOUS_IRQ)
    {
        if (irq_id == GIC_EXTI9_IRQ)
        {
            HAL_EXTI9_IRQHandler();
        }

        GICC_EOIR = interrupt_ack;
    }
}

/**
****************************************************************************
 * @fn HAL_CPU_EnableIrq
 * @brief 清除 A7 CPSR 的 IRQ 屏蔽位。
 * @param 无。
 * @return 无。
****************************************************************************
 */
void HAL_CPU_EnableIrq(void)
{
    __asm volatile ("cpsie i" ::: "memory");
}

/* Private function prototypes --------------------------------------------------------------------------*/

/* gic.c 文件结束。 */
