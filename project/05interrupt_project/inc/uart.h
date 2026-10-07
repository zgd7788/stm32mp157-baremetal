/**
****************************************************************************
 * @file uart.h
 * @brief 保留的 UART4 寄存器布局及轮询接口。
 * @version 1.2
 * @date 2026-10-05
 * @author Codex
 * @history 1.1: 按仓库编码规范整理。
 * @history 1.2: 改用中文函数注释，移除版权字段，按 Style 1.4 代码示例设置段落。
 * @attention Cortex-A7 裸机代码，规范偏离项见 Agent.md。
****************************************************************************
 */

/* Define to prevent recursive inclusion ----------------------------------------------------------------*/
#ifndef UART_H
#define UART_H

/* Includes ---------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "rcc.h"

/* Exported macros --------------------------------------------------------------------------------------*/
/* HSI must already be enabled; its frequency is a board-level precondition. */
#ifndef UART4_CLOCK_HZ
#define UART4_CLOCK_HZ 64000000U
#endif
#ifndef UART4_BAUDRATE
#define UART4_BAUDRATE 115200U
#endif

#define UART4_GPIO_AF 6U
#define UART_BRR_MIN 16U
#define UART_BRR_MAX 65535U
#define UART_DATA_MASK 0xFFU
#define UART_CR1_UE (1U << 0U)
#define UART_CR1_RE (1U << 2U)
#define UART_CR1_TE (1U << 3U)
#define UART_ISR_RXNE (1U << 5U)
#define UART_ISR_TC (1U << 6U)
#define UART_ISR_TXE (1U << 7U)
#define UART_ICR_CLEAR_FLAGS 0x5FU

/* Deviation: UART4 is a fixed peripheral register block. */
#define UART4 ((UART_REG_T *)0x40010000U)

/* Exported enum ----------------------------------------------------------------------------------------*/

/* Exported types ---------------------------------------------------------------------------------------*/
/* Register names follow the STM32MP157 reference manual. */
typedef struct
{
    volatile uint32_t CR1;  /* Enables and data format. */
    volatile uint32_t CR2;  /* Stop bits and clock format. */
    volatile uint32_t CR3;  /* Flow control and DMA. */
    volatile uint32_t BRR;  /* Baud rate divisor. */
    volatile uint32_t GTPR; /* Guard time and prescaler. */
    volatile uint32_t RTOR; /* Receiver timeout. */
    volatile uint32_t RQR;  /* Requests. */
    volatile uint32_t ISR;  /* Status flags. */
    volatile uint32_t ICR;  /* Flag clearing. */
    volatile uint32_t RDR;  /* Received data. */
    volatile uint32_t TDR;  /* Transmit data. */
} UART_REG_T;

/* Exported variables -----------------------------------------------------------------------------------*/

/* Exported functions -----------------------------------------------------------------------------------*/
/**
****************************************************************************
 * @fn HAL_UART4_Init
 * @brief 配置 PB2/PG11 为 AF6，并初始化 UART4 为 8-N-1 格式。
 * @param _baudrate 波特率；传入 0 时使用 UART4_BAUDRATE。
 * @return 无；波特率无法表示时不修改寄存器。
****************************************************************************
 */
void HAL_UART4_Init(
    uint32_t _baudrate    /* 请求的波特率。 */
);

/**
****************************************************************************
 * @fn HAL_UART4_PutChar
 * @brief 等待发送寄存器可用，并发送一个字节。
 * @param _data 待发送字节。
 * @return 无；调用前需完成 UART 初始化，本函数阻塞且无超时。
****************************************************************************
 */
void HAL_UART4_PutChar(
    uint8_t _data    /* 待发送字节。 */
);

/**
****************************************************************************
 * @fn HAL_UART4_PutString
 * @brief 发送以空字符结尾的字符串。
 * @param _string 待发送的有效字符串指针；NULL 不执行操作。
 * @return 无；调用前需完成 UART 初始化，本函数阻塞且无超时。
****************************************************************************
 */
void HAL_UART4_PutString(
    const char *_string    /* 待发送字符串。 */
);

/**
****************************************************************************
 * @fn HAL_UART4_GetChar
 * @brief 阻塞等待接收数据，并读取一个字节。
 * @param 无。
 * @return 接收到的字节；调用前需完成 UART 初始化，本函数无超时。
****************************************************************************
 */
uint8_t HAL_UART4_GetChar(void);

/**
****************************************************************************
 * @fn HAL_UART4_TryGetChar
 * @brief 以非阻塞方式读取接收数据。
 * @param _data 接收字节的有效输出指针，不能为 NULL。
 * @return 成功读取返回 1；无数据或指针为 NULL 时返回 0。
****************************************************************************
 */
int32_t HAL_UART4_TryGetChar(
    uint8_t *_data    /* 接收字节。 */
);

/**
****************************************************************************
 * @fn HAL_UART4_Flush
 * @brief 等待 UART4 的发送完成标志置位。
 * @param 无。
 * @return 无；调用前需完成 UART 初始化，本函数阻塞且无超时。
****************************************************************************
 */
void HAL_UART4_Flush(void);

#endif /* UART_H */

/* uart.h 文件结束。 */
