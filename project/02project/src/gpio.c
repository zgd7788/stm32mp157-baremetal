#include "/inc/gpio.h"

/**
 * @brief 遍历引脚掩码并设置选中引脚的 GPIO 配置。
 * @fn HAL_GPIO_Init
 * @param gpios 要配置的 GPIO 端口。
 * @param gpioinit 引脚掩码及模式、输出类型、速度和上下拉配置。
 * @return 无。
 */
void HAL_GPIO_Init(gpio_t *gpios, GPIO_INIT_T *gpioinit)
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
        }
    }
}

/**
 * @brief 按状态设置或清除端口输出数据寄存器中的指定引脚位。
 * @fn HAL_GPIO_Write
 * @param gpios 要写入的 GPIO 端口。
 * @param pins 要写入的引脚掩码。
 * @param statu 输出状态，GPIO_SET 为高电平，GPIO_RESET 为低电平。
 * @return 无。
 */
void HAL_GPIO_Write(gpio_t *gpios, unsigned int pins, GPIO_STATU_T statu)
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
 * @brief 根据输入数据寄存器读取指定引脚的电平状态。
 * @fn HAL_GPIO_Read
 * @param gpios 要读取的 GPIO 端口。
 * @param pins 要读取的引脚掩码。
 * @return 若任一选中引脚为高电平，返回 GPIO_SET；否则返回 GPIO_RESET。
 */
GPIO_STATU_E HAL_GPIO_Read(gpio_t *gpios, unsigned int pins)
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
