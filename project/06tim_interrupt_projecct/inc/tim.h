/**
****************************************************************************
 * @file tim.h
 * @brief TIM4_CH1 PWM 寄存器布局及公共接口。
 * @version 1.3
 * @date 2026-10-05
 * @author Codex
 * @history 1.1: 按仓库编码规范整理。
 * @history 1.2: 改用中文函数注释，移除版权字段，按 Style 1.4 代码示例设置段落。
 * @history 1.3: 增加 TIM4 更新中断接口。
 * @attention Cortex-A7 裸机代码，规范偏离项见 Agent.md。
****************************************************************************
 */

/* Define to prevent recursive inclusion ----------------------------------------------------------------*/
#ifndef TIM_H
#define TIM_H

/* Includes ---------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "rcc.h"

/* Exported macros --------------------------------------------------------------------------------------*/
/* The caller must supply the actual APB1 timer clock used by U-Boot. */
#ifndef TIM4_CLOCK_HZ
#define TIM4_CLOCK_HZ 64000000U
#endif

#define TIM_PWM_OK 0
#define TIM_PWM_ERROR (-1)
#define TIM_COUNTER_STATES 65536U
#define TIM_DUTY_MAX 100U
#define TIM4_GPIO_AF 2U
#define TIM4_INTERRUPT_FREQUENCY_HZ 1U

/* Deviation: TIM4 is a fixed hardware register block. */
#define TIM4 ((TIM_REG_T *)0x40002000U)
#define TIM_CR1_CEN (1U << 0U)
#define TIM_CR1_ARPE (1U << 7U)
#define TIM_EGR_UG (1U << 0U)
#define TIM_CCMR1_OC1PE (1U << 3U)
#define TIM_CCMR1_OC1M_INACTIVE (4U << 4U)
#define TIM_CCMR1_OC1M_ACTIVE (5U << 4U)
#define TIM_CCMR1_OC1M_PWM1 (6U << 4U)
#define TIM_CCER_CC1E (1U << 0U)
#define TIM_DIER_UIE (1U << 0U)
#define TIM_SR_UIF (1U << 0U)

/* Exported enum ----------------------------------------------------------------------------------------*/

/* Exported types ---------------------------------------------------------------------------------------*/
/* Register names and offsets follow the STM32MP157 reference manual. */
typedef struct
{
    volatile uint32_t CR1;   /* Counter control. */
    volatile uint32_t CR2;   /* Trigger control. */
    volatile uint32_t SMCR;  /* Slave mode. */
    volatile uint32_t DIER;  /* DMA and interrupt enables. */
    volatile uint32_t SR;    /* Status flags. */
    volatile uint32_t EGR;   /* Event generation. */
    volatile uint32_t CCMR1; /* Channel 1/2 mode. */
    volatile uint32_t CCMR2; /* Channel 3/4 mode. */
    volatile uint32_t CCER;  /* Channel enables and polarity. */
    volatile uint32_t CNT;   /* Counter value. */
    volatile uint32_t PSC;   /* Prescaler. */
    volatile uint32_t ARR;   /* Auto-reload value. */
    uint32_t reserved0;      /* Offset 0x30 is reserved. */
    volatile uint32_t CCR1;  /* Channel 1 compare value. */
    volatile uint32_t CCR2;  /* Channel 2 compare value. */
    volatile uint32_t CCR3;  /* Channel 3 compare value. */
    volatile uint32_t CCR4;  /* Channel 4 compare value. */
    uint32_t reserved1;      /* Offset 0x44 is reserved. */
    volatile uint32_t DCR;   /* DMA control. */
    volatile uint32_t DMAR;  /* DMA address. */
} TIM_REG_T;

/* Exported variables -----------------------------------------------------------------------------------*/

/* Exported functions -----------------------------------------------------------------------------------*/
/**
****************************************************************************
 * @fn HAL_TIM4_PWM_Init
 * @brief 配置 PB6 为 AF2，并初始化和启动 TIM4_CH1 PWM。
 * @param _frequency_hz PWM 频率，单位 Hz，必须可由 16 位定时器表示。
 * @param _duty_percent 占空比百分数，范围为 0～100。
 * @return 成功返回 TIM_PWM_OK；参数非法或频率无法表示时返回 TIM_PWM_ERROR，寄存器不变。
****************************************************************************
 */
int32_t HAL_TIM4_PWM_Init(
    uint32_t _frequency_hz,    /* PWM 频率，单位 Hz。 */
    uint32_t _duty_percent     /* 占空比百分数。 */
);

/**
****************************************************************************
 * @fn HAL_TIM4_PWM_Set
 * @brief 修改 PWM 频率和占空比，并保持原有启停状态。
 * @param _frequency_hz PWM 频率，单位 Hz，必须可由 16 位定时器表示。
 * @param _duty_percent 占空比百分数，范围为 0～100。
 * @return 成功返回 TIM_PWM_OK；未初始化或参数非法时返回 TIM_PWM_ERROR，寄存器不变。
****************************************************************************
 */
int32_t HAL_TIM4_PWM_Set(
    uint32_t _frequency_hz,    /* PWM 频率，单位 Hz。 */
    uint32_t _duty_percent     /* 占空比百分数。 */
);

/**
****************************************************************************
 * @fn HAL_TIM4_PWM_Enable
 * @brief 使能已初始化的 TIM4_CH1 输出并启动计数器。
 * @param 无。
 * @return 无；未初始化时不执行操作。
****************************************************************************
 */
void HAL_TIM4_PWM_Enable(void);

/**
****************************************************************************
 * @fn HAL_TIM4_PWM_Disable
 * @brief 关闭已初始化的 TIM4_CH1 输出并停止计数器。
 * @param 无。
 * @return 无；未初始化时不执行操作。
****************************************************************************
 */
void HAL_TIM4_PWM_Disable(void);

/**
****************************************************************************
 * @fn HAL_TIM4_InterruptInit
 * @brief 初始化 TIM4 更新中断，并以指定频率启动定时器。
 * @param _frequency_hz 更新中断频率，单位 Hz，必须可由 16 位计数器表示。
 * @return 成功返回 0；参数非法或频率无法表示时返回 -1。
****************************************************************************
 */
int32_t HAL_TIM4_InterruptInit(
    uint32_t _frequency_hz    /* 更新中断频率。 */
);

/**
****************************************************************************
 * @fn HAL_TIM4_IRQHandler
 * @brief 清除 TIM4 更新标志，并反转 PF8 LED。
 * @param 无。
 * @return 无；没有更新中断 pending 时不执行 LED 操作。
****************************************************************************
 */
void HAL_TIM4_IRQHandler(void);

#endif /* TIM_H */

/* tim.h 文件结束。 */
