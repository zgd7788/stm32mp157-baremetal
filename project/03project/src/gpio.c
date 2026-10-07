#include "gpio.h"

/**
 * @brief 按引脚编号选择 AFRL 或 AFRH 并更新复用字段。
 * @fn HAL_GPIO_SetAlternate
 * @param gpios 要配置的 GPIO 端口。
 * @param pins 要设置复用功能的引脚掩码。
 * @param alternate 复用功能编号，取值范围为 0 到 15。
 * @return 无。
 */
void HAL_GPIO_SetAlternate(gpio_t *gpios, uint32_t pins, uint32_t alternate)
{
    int pin_num;

    for (pin_num = 0; pin_num < 16; ++pin_num)
    {
        if ((pins & (1U << pin_num)) == 0U)
            continue;

        if (pin_num < 8)
        {
            uint32_t shift = (uint32_t)pin_num * 4U;
            gpios->AFRL = (gpios->AFRL & ~(0xFU << shift)) |
                          ((alternate & 0xFU) << shift);
        }
        else
        {
            uint32_t shift = (uint32_t)(pin_num - 8) * 4U;
            gpios->AFRH = (gpios->AFRH & ~(0xFU << shift)) |
                          ((alternate & 0xFU) << shift);
        }
    }
}

/**
 * @brief 配置掩码选中的引脚，并在复用模式下设置 AF 编号。
 * @fn HAL_GPIO_Init
 * @param gpios 要配置的 GPIO 端口。
 * @param gpioinit 引脚掩码、模式、电气属性和复用编号。
 * @return 无。
 */
void HAL_GPIO_Init(gpio_t *gpios, const GPIO_INIT_T *gpioinit)
{
    int pinNum;
    for(pinNum = 0; pinNum < 16; pinNum++)
    {
        if (gpioinit->pins & (0x1 << pinNum))
        {
            gpios->MODER &= ~(0x3 << (pinNum*2));
            gpios->MODER |= (gpioinit->mode << (pinNum*2));

            gpios->OTYPER &= ~(0x1 << pinNum);
            gpios->OTYPER |= (gpioinit->type << pinNum);

            gpios->OSPEEDR &=  ~(0x3 << (pinNum*2));
            gpios->OSPEEDR |= (gpioinit->speed << (pinNum*2));

            gpios->PUPDR &=  ~(0x3 << (pinNum*2));
            gpios->PUPDR |= (gpioinit->pupd << (pinNum*2));

            if (gpioinit->mode == GPIO_AF)
                HAL_GPIO_SetAlternate(gpios, 1U << pinNum, gpioinit->alternate);
        }
    }
}

/**
 * @brief 将指定引脚输出设置为高电平或低电平。
 * @fn HAL_GPIO_Write
 * @param gpios 要写入的 GPIO 端口。
 * @param pins 要写入的引脚掩码。
 * @param statu 输出状态，GPIO_SET 为高电平，GPIO_RESET 为低电平。
 * @return 无。
 */
void HAL_GPIO_Write(gpio_t *gpios, uint32_t pins, GPIO_STATU_T statu)
{
    if(statu != GPIO_RESET)
    {
        gpios->ODR |= pins;
    }
    else
    {
        gpios->ODR &= ~(pins);
    }
}

/**
 * @brief 检查输入数据寄存器中指定引脚的电平。
 * @fn HAL_GPIO_Read
 * @param gpios 要读取的 GPIO 端口。
 * @param pins 要读取的引脚掩码。
 * @return 任一选中引脚为高时返回 GPIO_SET，否则返回 GPIO_RESET。
 */
GPIO_STATU_T HAL_GPIO_Read(const gpio_t *gpios, uint32_t pins)
{
    if(gpios->IDR & pins)
    {
        return GPIO_SET;
    }
    else
    {
        return GPIO_RESET;
    }
}
