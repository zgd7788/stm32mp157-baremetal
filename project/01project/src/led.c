#include "/inc/led.h"

/**
 * @brief 使能 GPIOE/GPIOF 时钟，并将所选 LED 引脚配置为推挽输出。
 * @fn hal_led_init
 * @param leds 要初始化的 LED，可指定单个 LED 或 LED_ALL。
 * @return 无。
 */
void hal_led_init(LEDS_E leds)
{
    RCC_MP_AHB4ENSTER |= (0x01 << 4) |(0x01 << 5);

    switch (leds)
    {
        case LED1:
            GPIOE.MODER &= (~(0x3 << 20));
            GPIOE.MODER |= (0x1 << 20);
            GPIOE.OTYPER &= (~(0X1 << 10));
            GPIOE.OSPEEDR &= (~(0x3 << 20));
            GPIOE.PUPDR &= (~(0x3 << 20));
            break;
        case LED2:
            GPIOF.MODER &= (~(0x3 << 20));
            GPIOF.MODER |= (0x1 << 20);
            GPIOF.OTYPER &= (~(0X1 << 10));
            GPIOF.OSPEEDR &= (~(0x3 << 20));
            GPIOF.PUPDR &= (~(0x3 << 20));
            break;
        case LED3:
            GPIOF.MODER &= (~(0x3 << 16));
            GPIOF.MODER |= (0x1 << 16);
            GPIOF.OTYPER &= (~(0X1 << 8));
            GPIOF.OSPEEDR &= (~(0x3 << 16));
            GPIOF.PUPDR &= (~(0x3 << 16));
            break;
        case LED_ALL:
            hal_led_init(LED1);
            hal_led_init(LED2);
            hal_led_init(LED3);
            break;
        default:
        break;
    }
}

/**
 * @brief 根据 LED 名称和状态写入对应 GPIO 的输出数据寄存器。
 * @fn hal_led_switch
 * @param leds 要控制的 LED，可指定单个 LED 或 LED_ALL。
 * @param statu 输出状态，GPIO_SET 点亮，GPIO_RESET 熄灭。
 * @return 无。
 */
void hal_led_switch(LEDS_E leds, GPIO_STATU_E statu)
{
    switch (leds)
    {
    case LED1:
        if(statu == GPIO_SET)
        {
            GPIOE.ODR |= (0x1 << 10);
        }
        else
        {
            GPIOE.ODR &= (~(0x1 << 10));
        }
        break;
    case LED2:
        if(statu == GPIO_SET)
        {
            GPIOF.ODR |= (0x1 << 10);
        }
        else
        {
            GPIOF.ODR &= (~(0x1 << 10));
        }
        break;
    case LED3:
        if(statu == GPIO_SET)
        {
            GPIOF.ODR |= (0x1 << 8);
        }
        else
        {
            GPIOF.ODR &= (~(0x1 << 8));
        }
        break;
    case LED_ALL:
        if(statu == GPIO_SET)
        {
            GPIOE.ODR |= (0x1 << 10);
            GPIOF.ODR |= (0x1 << 10);
            GPIOF.ODR |= (0x1 << 8);
        }
        else
        {
            GPIOE.ODR &= (~(0x1 << 10));
            GPIOF.ODR &= (~(0x1 << 10));
            GPIOF.ODR &= (~(0x1 << 8));
        }
        break;
    default:
        break;
    }
}
