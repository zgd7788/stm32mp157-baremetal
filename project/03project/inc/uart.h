#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

/* UART4 is clocked from HSI by default, which is 64 MHz on STM32MP157. */
#ifndef UART4_CLOCK_HZ
#define UART4_CLOCK_HZ 64000000U
#endif

#ifndef UART4_BAUDRATE
#define UART4_BAUDRATE 115200U
#endif

#define RCC_MP_APB1ENSETR     (*(volatile uint32_t *)0x50000A00U)
#define RCC_UART24CKSELR      (*(volatile uint32_t *)0x500008E8U)

#define RCC_MP_APB1_UART4EN   (1U << 16)
#define RCC_UART24CLK_HSI     2U

typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t BRR;
    volatile uint32_t GTPR;
    volatile uint32_t RTOR;
    volatile uint32_t RQR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t RDR;
    volatile uint32_t TDR;
} uart_t;

#define UART4 ((uart_t *)0x40010000U)

#define UART_CR1_RE          (1U << 2)
#define UART_CR1_TE          (1U << 3)
#define UART_CR1_UE          (1U << 13)
#define UART_ISR_RXNE        (1U << 5)
#define UART_ISR_TC          (1U << 6)
#define UART_ISR_TXE         (1U << 7)

/**
 * @brief 初始化 UART4 引脚、时钟和串口格式。
 * @fn HAL_UART4_Init
 * @param baudrate 波特率；传入 0 时使用 UART4_BAUDRATE。
 * @return 无。
 */
void HAL_UART4_Init(uint32_t baudrate);
/**
 * @brief 阻塞发送一个字节，直到发送数据寄存器可写。
 * @fn HAL_UART4_PutChar
 * @param data 要发送的字节。
 * @return 无。
 */
void HAL_UART4_PutChar(uint8_t data);
/**
 * @brief 发送以空字符结尾的字符串。
 * @fn HAL_UART4_PutString
 * @param string 要发送的字符串；NULL 时不执行操作。
 * @return 无。
 */
void HAL_UART4_PutString(const char *string);
/**
 * @brief 阻塞等待并读取一个接收字节。
 * @fn HAL_UART4_GetChar
 * @param 无 本函数无参数。
 * @return 收到的一个字节。
 */
uint8_t HAL_UART4_GetChar(void);
/**
 * @brief 非阻塞读取一个接收字节。
 * @fn HAL_UART4_TryGetChar
 * @param data 用于接收数据的输出指针，不能为 NULL。
 * @return 成功读取时返回 1；无数据或 data 为 NULL 时返回 0。
 */
int HAL_UART4_TryGetChar(uint8_t *data);
/**
 * @brief 阻塞等待 UART4 完成当前及待发送数据。
 * @fn HAL_UART4_Flush
 * @param 无 本函数无参数。
 * @return 无。
 */
void HAL_UART4_Flush(void);

#endif
