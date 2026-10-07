/**
****************************************************************************
 * @file exti.c
 * @brief PF9 EXTI9 下降沿按键中断及 PF8 LED 反转实现。
 * @version 1.0
 * @date 2026-10-05
 * @author Codex
 * @history 1.0: 完成 PF9 下降沿中断和 PF8 LED 反转功能。
 * @attention Cortex-A7 裸机代码，规范偏离项见 Agent.md。
****************************************************************************
 */

/* Includes ---------------------------------------------------------------------------------------------*/
#include <stddef.h>
#include "exti.h"
#include "gpio.h"
#include "rcc.h"

/* Private macro ----------------------------------------------------------------------------------------*/
#define SYSCFG_EXTI9_PORT_F (5U << 4U)
#define SYSCFG_EXTI9_MASK (0xFU << 4U)

/* Private enum -----------------------------------------------------------------------------------------*/

/* Private typedef --------------------------------------------------------------------------------------*/

/* Exported variables -----------------------------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------------------------------*/

/* Private function declaration -------------------------------------------------------------------------*/
static EXTI_REG_T *EXTI_GetRegister(void);
static SYSCFG_REG_T *SYSCFG_GetRegister(void);

/* Exported function declaration ------------------------------------------------------------------------*/
/* 本模块接口已在 exti.h 中声明。 */

/* Exported functions、 prototypes-------------------------------------------------------------------------*/
/**
****************************************************************************
 * @fn HAL_EXTI9_Init
 * @brief 配置 PF9 为 EXTI9 下降沿中断输入并准备 PF8 LED 输出。
 * @param 无。
 * @return 成功返回 0；外设配置失败返回 -1。
****************************************************************************
 */
int32_t HAL_EXTI9_Init(void)
{
    const GPIO_INIT_T led_init =
    {
        GPIO_PIN_8,
        GPIO_OUTPUT,
        GPIO_PP,
        GPIO_LOW_LEVEL,
        GPIO_NOPUPD,
        0U
    };
    const GPIO_INIT_T key_init =
    {
        GPIO_PIN_9,
        GPIO_INPUT,
        GPIO_PP,
        GPIO_LOW_LEVEL,
        GPIO_PU,
        0U
    };
    EXTI_REG_T *exti = EXTI_GetRegister();
    SYSCFG_REG_T *syscfg = SYSCFG_GetRegister();
    int32_t status = -1;

    if ((exti != NULL) && (syscfg != NULL))
    {
        RCC_MP_AHB4ENSETR |= RCC_MP_AHB4_GPIOFEN;
        RCC_MP_APB3ENSETR |= RCC_MP_APB3_SYSCFGEN;
        HAL_GPIO_Init(GPIOF, &led_init);
        HAL_GPIO_Init(GPIOF, &key_init);
        HAL_GPIO_Write(GPIOF, GPIO_PIN_8, GPIO_RESET);

        syscfg->EXTICR3 = (syscfg->EXTICR3 & ~SYSCFG_EXTI9_MASK)
            | SYSCFG_EXTI9_PORT_F;
        exti->RTSR1 &= ~EXTI9_LINE;
        exti->FTSR1 |= EXTI9_LINE;
        exti->RPR1 = EXTI9_LINE;
        exti->FPR1 = EXTI9_LINE;
        exti->C1PR1 = EXTI9_LINE;
        exti->C1IMR1 |= EXTI9_LINE;
        status = 0;
    }

    return status;
}

/**
****************************************************************************
 * @fn HAL_EXTI9_IRQHandler
 * @brief 处理 EXTI9 下降沿 pending，并反转 PF8 LED。
 * @param 无。
 * @return 无；无 EXTI9 pending 时不执行 LED 操作。
****************************************************************************
 */
void HAL_EXTI9_IRQHandler(void)
{
    EXTI_REG_T *exti = EXTI_GetRegister();

    if ((exti != NULL) && ((exti->C1PR1 & EXTI9_LINE) != 0U))
    {
        exti->FPR1 = EXTI9_LINE;
        exti->C1PR1 = EXTI9_LINE;
        HAL_GPIO_Toggle(GPIOF, GPIO_PIN_8);
    }
}

/* Private function prototypes --------------------------------------------------------------------------*/
/**
****************************************************************************
 * @fn EXTI_GetRegister
 * @brief 返回 EXTI 寄存器块地址。
 * @param 无。
 * @return EXTI 寄存器块指针。
****************************************************************************
 */
static EXTI_REG_T *EXTI_GetRegister(void)
{
    return (EXTI_REG_T *)EXTI_BASE_ADDRESS;
}

/**
****************************************************************************
 * @fn SYSCFG_GetRegister
 * @brief 返回 SYSCFG 寄存器块地址。
 * @param 无。
 * @return SYSCFG 寄存器块指针。
****************************************************************************
 */
static SYSCFG_REG_T *SYSCFG_GetRegister(void)
{
    return (SYSCFG_REG_T *)SYSCFG_BASE_ADDRESS;
}

/* exti.c 文件结束。 */
