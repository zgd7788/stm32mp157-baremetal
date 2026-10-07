#define RCC_MP_AHB4ENSTER *(volatile unsigned int *)0x50000A28

#define GPIOA_MODER *(volatile unsigned int *)0x50002000
#define GPIOA_OTYPER *(volatile unsigned int *)0x50002004
#define GPIOA_OSPEEDR *(volatile unsigned int *)0x50002008
#define GPIOA_PUPDR *(volatile unsigned int *)0x5000200C
#define GPIOA_IDR *(volatile unsigned int *)0x50002010
#define GPIOA_ODR *(volatile unsigned int *)0x50002014

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


