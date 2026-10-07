#ifndef __LED_H__
#define __LED_H__

#define RCC_MP_AHB4ENSTER *(volatile unsigned int *)0x50000A28
typedef struct 
{
    volatile unsigned int MODER;
    volatile unsigned int OTYPER;
    volatile unsigned int OSPEEDR;
    volatile unsigned int PUPDR;
    volatile unsigned int IDR;
    volatile unsigned int ODR;
}gpio_t;

#define GPIOA (*(gpio_t *)0x50002000)
#define GPIOB (*(gpio_t *)0x50003000)
#define GPIOC (*(gpio_t *)0x50004000)
#define GPIOD (*(gpio_t *)0x50005000)
#define GPIOE (*(gpio_t *)0x50006000)
#define GPIOF (*(gpio_t *)0x50007000)

typedef enum
{
    LED1 = 1,
    LED2,
    LED3,
    LED_ALL,
}LEDS_E;

typedef enum
{
    GPIO_RESET = 0,
    GPIO_SET,
}GPIO_STATU_E;


/**
 * @brief 初始化指定 LED 对应的 GPIO 输出引脚。
 * @fn hal_led_init
 * @param leds 要初始化的 LED，可指定单个 LED 或 LED_ALL。
 * @return 无。
 */
void hal_led_init(LEDS_E leds);
/**
 * @brief 设置指定 LED 的输出状态。
 * @fn hal_led_switch
 * @param leds 要控制的 LED，可指定单个 LED 或 LED_ALL。
 * @param statu 输出状态，GPIO_SET 点亮，GPIO_RESET 熄灭。
 * @return 无。
 */
void hal_led_switch(LEDS_E leds, GPIO_STATU_E statu);



#endif
