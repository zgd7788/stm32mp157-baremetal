#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>

#define RCC_MP_AHB4ENSETR (*(volatile uint32_t *)0x50000A28U)
/* Keep the original spelling used by the earlier GPIO exercise. */
#define RCC_MP_AHB4ENSTER RCC_MP_AHB4ENSETR

typedef struct
{
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t RESERVED[2];
    volatile uint32_t AFRL;
    volatile uint32_t AFRH;
} gpio_t;

#define GPIOA ((gpio_t *)0x50002000)
#define GPIOB ((gpio_t *)0x50003000)
#define GPIOC ((gpio_t *)0x50004000)
#define GPIOD ((gpio_t *)0x50005000)
#define GPIOE ((gpio_t *)0x50006000)
#define GPIOF ((gpio_t *)0x50007000)
#define GPIOG ((gpio_t *)0x50008000)
#define GPIOH ((gpio_t *)0x50009000)
#define GPIOI ((gpio_t *)0x5000A000)
#define GPIOJ ((gpio_t *)0x5000B000)
#define GPIOK ((gpio_t *)0x5000C000)

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

typedef enum
{
    GPIO_INPUT = 0,
    GPIO_OUTPUT,
    GPIO_AF,
    GPIO_ANALOG,
}GPIO_MODER_E;

typedef enum
{
    GPIO_PP = 0,
    GPIO_OD,
}GPIO_OTYPER_E;

typedef enum
{
    GPIO_LOW_LEVEL = 0,
    GPIO_MEDIUM_LEVEL,
    GPIO_HIGH_LEVEL,
    GPIO_VERY_HIGH_LEVEL,
}GPIO_OSPEEDR_E;

typedef enum
{
    GPIO_NOPUPD = 0,
    GPIO_PU,
    GPIO_PD,
}GPIO_PUPDR_E;

typedef struct 
{
    uint32_t pins;
    GPIO_MODER_E mode;
    GPIO_OTYPER_E type;
    GPIO_OSPEEDR_E speed;
    GPIO_PUPDR_E pupd;
    uint32_t alternate;
}GPIO_INIT_T;

typedef enum
{
    GPIO_RESET = 0,
    GPIO_SET,
}GPIO_STATU_T;

/**
 * @brief 配置引脚模式、电气属性，并在复用模式下写入 AF 编号。
 * @fn HAL_GPIO_Init
 * @param gpios 要配置的 GPIO 端口。
 * @param gpioinit 引脚掩码、模式、电气属性和复用编号。
 * @return 无。
 */
void HAL_GPIO_Init(gpio_t *gpios, const GPIO_INIT_T *gpioinit);
/**
 * @brief 为掩码选中的引脚设置 GPIO 复用功能编号。
 * @fn HAL_GPIO_SetAlternate
 * @param gpios 要配置的 GPIO 端口。
 * @param pins 要设置复用功能的引脚掩码。
 * @param alternate 复用功能编号，取值范围为 0 到 15。
 * @return 无。
 */
void HAL_GPIO_SetAlternate(gpio_t *gpios, uint32_t pins, uint32_t alternate);
/**
 * @brief 将掩码选中的引脚输出设为高电平或低电平。
 * @fn HAL_GPIO_Write
 * @param gpios 要写入的 GPIO 端口。
 * @param pins 要写入的引脚掩码。
 * @param statu 输出状态，GPIO_SET 为高电平，GPIO_RESET 为低电平。
 * @return 无。
 */
void HAL_GPIO_Write(gpio_t *gpios, uint32_t pins, GPIO_STATU_T statu);
/**
 * @brief 读取掩码对应的输入电平。
 * @fn HAL_GPIO_Read
 * @param gpios 要读取的 GPIO 端口。
 * @param pins 要读取的引脚掩码。
 * @return 任一选中引脚为高时返回 GPIO_SET，否则返回 GPIO_RESET。
 */
GPIO_STATU_T HAL_GPIO_Read(const gpio_t *gpios, uint32_t pins);

#endif
