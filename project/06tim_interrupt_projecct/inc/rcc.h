/**
****************************************************************************
 * @file rcc.h
 * @brief 公共 RCC 寄存器地址及外设时钟使能位。
 * @version 1.2
 * @date 2026-10-05
 * @author Codex
 * @history 1.1: 按仓库编码规范整理。
 * @history 1.2: 改用中文函数注释，移除版权字段，按 Style 1.4 代码示例设置段落。
 * @attention Cortex-A7 裸机代码，规范偏离项见 Agent.md。
****************************************************************************
 */

/* Define to prevent recursive inclusion ----------------------------------------------------------------*/
#ifndef RCC_H
#define RCC_H

/* Includes ---------------------------------------------------------------------------------------------*/
#include <stdint.h>

/* Exported macros --------------------------------------------------------------------------------------*/
/* Deviation: fixed MMIO addresses require integer-to-pointer conversions. */
#define RCC_MP_AHB4ENSETR (*(volatile uint32_t *)0x50000A28U)
#define RCC_MP_APB1ENSETR (*(volatile uint32_t *)0x50000A00U)
#define RCC_MP_APB3ENSETR (*(volatile uint32_t *)0x50000A10U)
#define RCC_UART24CKSELR (*(volatile uint32_t *)0x500008E8U)

#define RCC_MP_AHB4_GPIOBEN (1U << 1U)
#define RCC_MP_AHB4_GPIOFEN (1U << 5U)
#define RCC_MP_AHB4_GPIOGEN (1U << 6U)
#define RCC_MP_APB1_TIM4EN (1U << 2U)
#define RCC_MP_APB1_UART4EN (1U << 16U)
#define RCC_MP_APB3_SYSCFGEN (1U << 0U)
#define RCC_UART24CLK_HSI 2U

/* Exported enum ----------------------------------------------------------------------------------------*/

/* Exported types ---------------------------------------------------------------------------------------*/

/* Exported variables -----------------------------------------------------------------------------------*/

/* Exported functions -----------------------------------------------------------------------------------*/

#endif /* RCC_H */

/* rcc.h 文件结束。 */
