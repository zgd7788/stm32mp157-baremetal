/**
****************************************************************************
 * @file gpio.h
 * @brief GPIO 寄存器布局、引脚配置及公共接口。
 * @version 1.2
 * @date 2026-10-05
 * @author Codex
 * @history 1.1: 按仓库编码规范整理。
 * @history 1.2: 改用中文函数注释，移除版权字段，按 Style 1.4 代码示例设置段落。
 * @attention Cortex-A7 裸机代码，规范偏离项见 Agent.md。
****************************************************************************
 */

/* Define to prevent recursive inclusion ----------------------------------------------------------------*/
#ifndef GPIO_H
#define GPIO_H

/* Includes ---------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "rcc.h"

/* Exported macros --------------------------------------------------------------------------------------*/
#define GPIO_PIN_0 0x0001U
#define GPIO_PIN_1 0x0002U
#define GPIO_PIN_2 0x0004U
#define GPIO_PIN_3 0x0008U
#define GPIO_PIN_4 0x0010U
#define GPIO_PIN_5 0x0020U
#define GPIO_PIN_6 0x0040U
#define GPIO_PIN_7 0x0080U
#define GPIO_PIN_8 0x0100U
#define GPIO_PIN_9 0x0200U
#define GPIO_PIN_10 0x0400U
#define GPIO_PIN_11 0x0800U
#define GPIO_PIN_12 0x1000U
#define GPIO_PIN_13 0x2000U
#define GPIO_PIN_14 0x4000U
#define GPIO_PIN_15 0x8000U
#define GPIO_PIN_MASK 0xFFFFU
#define GPIO_PIN_COUNT 16U
#define GPIO_AF_PIN_COUNT 8U
#define GPIO_AF_MAX 15U
#define GPIO_MODE_MASK 3U
#define GPIO_MODE_WIDTH 2U
#define GPIO_AF_WIDTH 4U

/* Deviation: these pointers identify fixed peripheral register blocks. */
#define GPIOA ((GPIO_REG_T *)0x50002000U)
#define GPIOB ((GPIO_REG_T *)0x50003000U)
#define GPIOC ((GPIO_REG_T *)0x50004000U)
#define GPIOD ((GPIO_REG_T *)0x50005000U)
#define GPIOE ((GPIO_REG_T *)0x50006000U)
#define GPIOF ((GPIO_REG_T *)0x50007000U)
#define GPIOG ((GPIO_REG_T *)0x50008000U)
#define GPIOH ((GPIO_REG_T *)0x50009000U)
#define GPIOI ((GPIO_REG_T *)0x5000A000U)
#define GPIOJ ((GPIO_REG_T *)0x5000B000U)
#define GPIOK ((GPIO_REG_T *)0x5000C000U)

/* Exported enum ----------------------------------------------------------------------------------------*/
typedef enum
{
    GPIO_INPUT = 0,
    GPIO_OUTPUT,
    GPIO_AF,
    GPIO_ANALOG
} GPIO_MODER_E;

typedef enum
{
    GPIO_PP = 0,
    GPIO_OD
} GPIO_OTYPER_E;

typedef enum
{
    GPIO_LOW_LEVEL = 0,
    GPIO_MEDIUM_LEVEL,
    GPIO_HIGH_LEVEL,
    GPIO_VERY_HIGH_LEVEL
} GPIO_OSPEEDR_E;

typedef enum
{
    GPIO_NOPUPD = 0,
    GPIO_PU,
    GPIO_PD
} GPIO_PUPDR_E;

typedef enum
{
    GPIO_RESET = 0,
    GPIO_SET
} GPIO_STATE_E;

/* Exported types ---------------------------------------------------------------------------------------*/
/* Register names follow the reference manual; layout must not be reordered. */
typedef struct
{
    volatile uint32_t MODER;   /* Pin modes. */
    volatile uint32_t OTYPER;  /* Output types. */
    volatile uint32_t OSPEEDR; /* Output speeds. */
    volatile uint32_t PUPDR;   /* Pull resistors. */
    volatile uint32_t IDR;     /* Input levels. */
    volatile uint32_t ODR;     /* Output levels. */
    uint32_t reserved[2];      /* BSRR and LCKR are not used by this driver. */
    volatile uint32_t AFRL;    /* Alternate functions for pins 0..7. */
    volatile uint32_t AFRH;    /* Alternate functions for pins 8..15. */
} GPIO_REG_T;

typedef struct
{
    uint32_t pins;         /* Nonzero mask of pins 0..15. */
    GPIO_MODER_E mode;     /* Input, output, alternate or analog mode. */
    GPIO_OTYPER_E type;    /* Push-pull or open-drain output. */
    GPIO_OSPEEDR_E speed;  /* Output slew rate. */
    GPIO_PUPDR_E pupd;     /* Pull-up/pull-down selection. */
    uint32_t alternate;    /* Alternate function index, 0..15. */
} GPIO_INIT_T;

/* Exported variables -----------------------------------------------------------------------------------*/

/* Exported functions -----------------------------------------------------------------------------------*/
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
);

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
);

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
);

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
);

#endif /* GPIO_H */

/* gpio.h 文件结束。 */
