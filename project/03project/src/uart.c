#include "gpio.h"
#include "uart.h"

/**
 * @brief 使能 GPIOB/GPIOG 时钟，并将 PB2、PG11 配置为 UART4 AF6。
 * @fn uart4_gpio_init
 * @param 无 本函数无参数。
 * @return 无。
 */
static void uart4_gpio_init(void)
{
    GPIO_INIT_T rx_init = {
        GPIO_PIN_2,
        GPIO_AF,
        GPIO_PP,
        GPIO_HIGH_LEVEL,
        GPIO_PU,
        6U,
    };
    GPIO_INIT_T tx_init = {
        GPIO_PIN_11,
        GPIO_AF,
        GPIO_PP,
        GPIO_HIGH_LEVEL,
        GPIO_PU,
        6U,
    };

    RCC_MP_AHB4ENSETR |= (1U << 1) | (1U << 6); /* GPIOB and GPIOG */
    HAL_GPIO_Init(GPIOB, &rx_init);             /* PB2: UART4_RX */
    HAL_GPIO_Init(GPIOG, &tx_init);             /* PG11: UART4_TX */
}

/**
 * @brief 选择 HSI 内核时钟，配置波特率及 8-N-1 收发模式并使能 UART4。
 * @fn HAL_UART4_Init
 * @param baudrate 波特率；传入 0 时使用默认波特率。
 * @return 无。
 */
void HAL_UART4_Init(uint32_t baudrate)
{
    uint32_t brr;

    if (baudrate == 0U)
        baudrate = UART4_BAUDRATE;

    uart4_gpio_init();

    RCC_MP_APB1ENSETR |= RCC_MP_APB1_UART4EN;
    RCC_UART24CKSELR = RCC_UART24CLK_HSI;

    /* Disable the peripheral while changing its format and baud rate. */
    UART4->CR1 = 0U;
    UART4->CR2 = 0U; /* 1 stop bit */
    UART4->CR3 = 0U; /* no flow control, FIFO disabled */

    brr = (UART4_CLOCK_HZ + baudrate / 2U) / baudrate;
    UART4->BRR = brr;
    UART4->ICR = 0xFFFFFFFFU;
    UART4->CR1 = UART_CR1_RE | UART_CR1_TE | UART_CR1_UE;
}

/**
 * @brief 等待发送数据寄存器可写，然后发送一个字节。
 * @fn HAL_UART4_PutChar
 * @param data 要发送的字节。
 * @return 无。
 */
void HAL_UART4_PutChar(uint8_t data)
{
    while ((UART4->ISR & UART_ISR_TXE) == 0U)
        ;

    UART4->TDR = data;
}

/**
 * @brief 逐字节发送以空字符结尾的字符串。
 * @fn HAL_UART4_PutString
 * @param string 要发送的字符串；NULL 时不执行操作。
 * @return 无。
 */
void HAL_UART4_PutString(const char *string)
{
    if (string == 0)
        return;

    while (*string != '\0')
        HAL_UART4_PutChar((uint8_t)*string++);
}

/**
 * @brief 等待接收数据到达，并从接收数据寄存器读取一个字节。
 * @fn HAL_UART4_GetChar
 * @param 无 本函数无参数。
 * @return 收到的一个字节。
 */
uint8_t HAL_UART4_GetChar(void)
{
    while ((UART4->ISR & UART_ISR_RXNE) == 0U)
        ;

    return (uint8_t)UART4->RDR;
}

/**
 * @brief 非阻塞检查接收状态，有数据时写入输出参数。
 * @fn HAL_UART4_TryGetChar
 * @param data 用于接收数据的输出指针，不能为 NULL。
 * @return 成功读取时返回 1；无数据或 data 为 NULL 时返回 0。
 */
int HAL_UART4_TryGetChar(uint8_t *data)
{
    if ((data == 0) || ((UART4->ISR & UART_ISR_RXNE) == 0U))
        return 0;

    *data = (uint8_t)UART4->RDR;
    return 1;
}

/**
 * @brief 等待 UART4 的发送完成标志置位。
 * @fn HAL_UART4_Flush
 * @param 无 本函数无参数。
 * @return 无。
 */
void HAL_UART4_Flush(void)
{
    while ((UART4->ISR & UART_ISR_TC) == 0U)
        ;
}
