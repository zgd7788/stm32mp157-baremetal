/**
****************************************************************************
 * @file exti.h
 * @brief PF9 下降沿 EXTI9 按键中断接口。
 * @version 1.0
 * @date 2026-10-05
 * @author Codex
 * @history 1.0: 完成 PF9 下降沿中断和 PF8 LED 反转功能。
 * @attention Cortex-A7 裸机代码，规范偏离项见 Agent.md。
****************************************************************************
 */

/* Define to prevent recursive inclusion ----------------------------------------------------------------*/
#ifndef EXTI_H
#define EXTI_H

/* Includes ---------------------------------------------------------------------------------------------*/
#include <stdint.h>

/* Exported macros --------------------------------------------------------------------------------------*/
#define EXTI9_LINE (1U << 9U)
#define EXTI9_GIC_IRQ 23U
#define EXTI_BASE_ADDRESS 0x5000D000U
#define SYSCFG_BASE_ADDRESS 0x50020000U

/* Exported enum ----------------------------------------------------------------------------------------*/

/* Exported types ---------------------------------------------------------------------------------------*/
typedef struct
{
    volatile uint32_t RTSR1;
    volatile uint32_t FTSR1;
    volatile uint32_t SWIER1;
    volatile uint32_t RPR1;
    volatile uint32_t FPR1;
    volatile uint32_t SECCFGR1;
    volatile uint32_t PRIVCFGR1;
    volatile uint32_t D3PMR1;
    uint32_t reserved0[24];
    volatile uint32_t C1IMR1;
    volatile uint32_t C1EMR1;
    volatile uint32_t C1PR1;
} EXTI_REG_T;

typedef struct
{
    uint32_t reserved0[2];
    volatile uint32_t EXTICR3;
} SYSCFG_REG_T;

/* Exported variables -----------------------------------------------------------------------------------*/

/* Exported functions -----------------------------------------------------------------------------------*/
/**
****************************************************************************
 * @fn HAL_EXTI9_Init
 * @brief 配置 PF9 为 EXTI9 下降沿中断输入并准备 PF8 LED 输出。
 * @param 无。
 * @return 成功返回 0；外设配置失败返回 -1。
****************************************************************************
 */
int32_t HAL_EXTI9_Init(void);

/**
****************************************************************************
 * @fn HAL_EXTI9_IRQHandler
 * @brief 处理 EXTI9 下降沿 pending，并反转 PF8 LED。
 * @param 无。
 * @return 无；无 EXTI9 pending 时不执行 LED 操作。
****************************************************************************
 */
void HAL_EXTI9_IRQHandler(void);

#endif /* EXTI_H */

/* exti.h 文件结束。 */
