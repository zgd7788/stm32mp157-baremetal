/**
****************************************************************************
 * @file gpio.c
 * @brief GPIO 配置及输入输出访问。
 * @version 1.2
 * @date 2026-10-05
 * @author Codex
 * @history 1.1: 按仓库编码规范整理。
 * @history 1.2: 改用中文函数注释，移除版权字段，按 Style 1.4 代码示例设置段落。
 * @attention Cortex-A7 裸机代码，规范偏离项见 Agent.md。
****************************************************************************
 */

/* Includes ---------------------------------------------------------------------------------------------*/
#include <stddef.h>
#include "gpio.h"

/* Private macro ----------------------------------------------------------------------------------------*/

/* Private enum -----------------------------------------------------------------------------------------*/

/* Private typedef --------------------------------------------------------------------------------------*/

/* Exported variables -----------------------------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------------------------------*/

/* Private function declaration -------------------------------------------------------------------------*/

/* Exported function declaration ------------------------------------------------------------------------*/
/* 本模块接口已在 gpio.h 中声明。 */

/* Exported functions、 prototypes-------------------------------------------------------------------------*/
/**
****************************************************************************
 * @fn HAL_GPIO_Init
 * @brief 配置指定 GPIO 引脚的模式、电气属性及复用功能。
 * @param _gpios GPIO 寄存器指针，端口时钟需预先使能。
 * @param _gpio_init 引脚配置指针；NULL 或非法配置不执行寄存器写入。
 * @return 无。
****************************************************************************
 */
void HAL_GPIO_Init(
    GPIO_REG_T *_gpios,              /* 目标 GPIO 端口。 */
    const GPIO_INIT_T *_gpio_init    /* 引脚配置。 */
)
{
    uint32_t pin;   /* Pin index, limited to the register width. */
    uint32_t shift; /* Two-bit configuration field position. */

    /* Reject the entire request before making any register changes. */
    if ((_gpios != NULL) && (_gpio_init != NULL))
    {
        if ((_gpio_init->pins != 0U)
            && ((_gpio_init->pins & ~GPIO_PIN_MASK) == 0U)
            && ((uint32_t)_gpio_init->mode <= (uint32_t)GPIO_ANALOG)
            && ((uint32_t)_gpio_init->type <= (uint32_t)GPIO_OD)
            && ((uint32_t)_gpio_init->speed <= (uint32_t)GPIO_VERY_HIGH_LEVEL)
            && ((uint32_t)_gpio_init->pupd <= (uint32_t)GPIO_PD)
            && (_gpio_init->alternate <= GPIO_AF_MAX))
        {
            /* All shifts use unsigned operands, including the pin 15 field. */
            for (pin = 0U; pin < GPIO_PIN_COUNT; ++pin)
            {
                if ((_gpio_init->pins & (1U << pin)) != 0U)
                {
                    shift = pin * GPIO_MODE_WIDTH;
                    _gpios->MODER = (_gpios->MODER & ~(GPIO_MODE_MASK << shift))
                        | ((uint32_t)_gpio_init->mode << shift);
                    _gpios->OTYPER = (_gpios->OTYPER & ~(1U << pin))
                        | ((uint32_t)_gpio_init->type << pin);
                    _gpios->OSPEEDR = (_gpios->OSPEEDR & ~(GPIO_MODE_MASK << shift))
                        | ((uint32_t)_gpio_init->speed << shift);
                    _gpios->PUPDR = (_gpios->PUPDR & ~(GPIO_MODE_MASK << shift))
                        | ((uint32_t)_gpio_init->pupd << shift);
                }
            }

            if (_gpio_init->mode == GPIO_AF)
            {
                HAL_GPIO_SetAlternate(_gpios, _gpio_init->pins,
                    _gpio_init->alternate);
            }
        }
    }
}

/**
****************************************************************************
 * @fn HAL_GPIO_SetAlternate
 * @brief 为指定引脚设置复用功能编号。
 * @param _gpios GPIO 寄存器指针，端口时钟需预先使能。
 * @param _pins 非零的 16 位引脚掩码。
 * @param _alternate 复用功能编号，范围为 0～15。
 * @return 无；非法参数不执行寄存器写入。
****************************************************************************
 */
void HAL_GPIO_SetAlternate(
    GPIO_REG_T *_gpios,    /* 目标 GPIO 端口。 */
    uint32_t _pins,        /* 选中引脚掩码。 */
    uint32_t _alternate    /* 复用功能编号。 */
)
{
    uint32_t pin;   /* Selected pin index. */
    uint32_t shift; /* Four-bit alternate function field position. */

    if ((_gpios != NULL) && (_pins != 0U)
        && ((_pins & ~GPIO_PIN_MASK) == 0U) && (_alternate <= GPIO_AF_MAX))
    {
        for (pin = 0U; pin < GPIO_PIN_COUNT; ++pin)
        {
            if ((_pins & (1U << pin)) != 0U)
            {
                /* The lower and upper pin banks have separate AF registers. */
                if (pin < GPIO_AF_PIN_COUNT)
                {
                    shift = pin * GPIO_AF_WIDTH;
                    _gpios->AFRL = (_gpios->AFRL & ~(GPIO_AF_MAX << shift))
                        | (_alternate << shift);
                }
                else
                {
                    shift = (pin - GPIO_AF_PIN_COUNT) * GPIO_AF_WIDTH;
                    _gpios->AFRH = (_gpios->AFRH & ~(GPIO_AF_MAX << shift))
                        | (_alternate << shift);
                }
            }
        }
    }
}

/**
****************************************************************************
 * @fn HAL_GPIO_Write
 * @brief 设置指定引脚的输出电平。
 * @param _gpios GPIO 寄存器指针，端口时钟需预先使能。
 * @param _pins 非零的 16 位引脚掩码。
 * @param _state 输出状态，GPIO_SET 为高，GPIO_RESET 为低。
 * @return 无；非法参数不执行寄存器写入。
****************************************************************************
 */
void HAL_GPIO_Write(
    GPIO_REG_T *_gpios,    /* 目标 GPIO 端口。 */
    uint32_t _pins,        /* 选中引脚掩码。 */
    GPIO_STATE_E _state   /* 输出电平。 */
)
{
    if ((_gpios != NULL) && (_pins != 0U)
        && ((_pins & ~GPIO_PIN_MASK) == 0U))
    {
        /* This read-modify-write API requires exclusive access to the port. */
        if (_state == GPIO_SET)
        {
            _gpios->ODR |= _pins;
        }
        else if (_state == GPIO_RESET)
        {
            _gpios->ODR &= ~_pins;
        }
        else
        {
            /* Invalid enum values leave the output unchanged. */
        }
    }
}

/**
****************************************************************************
 * @fn HAL_GPIO_Toggle
 * @brief 反转指定 GPIO 输出引脚的电平。
 * @param _gpios GPIO 寄存器指针，端口时钟需预先使能。
 * @param _pins 非零的 16 位引脚掩码。
 * @return 无；非法参数不执行寄存器写入。
****************************************************************************
 */
void HAL_GPIO_Toggle(
    GPIO_REG_T *_gpios,    /* 目标 GPIO 端口。 */
    uint32_t _pins         /* 选中引脚掩码。 */
)
{
    if ((_gpios != NULL) && (_pins != 0U)
        && ((_pins & ~GPIO_PIN_MASK) == 0U))
    {
        /* This read-modify-write API requires exclusive access to the port. */
        _gpios->ODR ^= _pins;
    }
}

/**
****************************************************************************
 * @fn HAL_GPIO_Read
 * @brief 读取指定引脚的输入电平。
 * @param _gpios GPIO 寄存器指针，端口时钟需预先使能。
 * @param _pins 非零的 16 位引脚掩码。
 * @return 任一选中引脚为高时返回 GPIO_SET，否则或参数非法时返回 GPIO_RESET。
****************************************************************************
 */
GPIO_STATE_E HAL_GPIO_Read(
    const GPIO_REG_T *_gpios,    /* 目标 GPIO 端口。 */
    uint32_t _pins               /* 选中引脚掩码。 */
)
{
    GPIO_STATE_E state = GPIO_RESET; /* Default for low or invalid input. */

    if ((_gpios != NULL) && (_pins != 0U)
        && ((_pins & ~GPIO_PIN_MASK) == 0U))
    {
        if ((_gpios->IDR & _pins) != 0U)
        {
            state = GPIO_SET;
        }
    }

    return state;
}

/* Private function prototypes --------------------------------------------------------------------------*/

/* gpio.c 文件结束。 */
