/**
****************************************************************************
 * @file uart.c
 * @brief 保留的 UART4 轮询驱动，默认蜂鸣器构建不使用。
 * @version 1.2
 * @date 2026-10-05
 * @author Codex
 * @history 1.1: 按仓库编码规范整理。
 * @history 1.2: 改用中文函数注释，移除版权字段，按 Style 1.4 代码示例设置段落。
 * @attention Cortex-A7 裸机代码，规范偏离项见 Agent.md。
****************************************************************************
 */

/* Includes ---------------------------------------------------------------------------------------------*/
#include <stddef.h>
#include "gpio.h"
#include "uart.h"

/* Private macro ----------------------------------------------------------------------------------------*/

/* Private enum -----------------------------------------------------------------------------------------*/

/* Private typedef --------------------------------------------------------------------------------------*/
_Static_assert(offsetof(UART_REG_T, TDR) == 0x28U, "UART4 layout mismatch");

/* Exported variables -----------------------------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------------------------------*/

/* Private function declaration -------------------------------------------------------------------------*/
static void UART4_InitGpio(void);

/* Exported function declaration ------------------------------------------------------------------------*/
/* 本模块接口已在 uart.h 中声明。 */

/* Exported functions、 prototypes-------------------------------------------------------------------------*/
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
)
{
    uint32_t baudrate = _baudrate; /* Local copy preserves the input parameter. */
    uint64_t divisor; /* Wide rounding calculation avoids addition overflow. */

    if (baudrate == 0U)
    {
        baudrate = UART4_BAUDRATE;
    }

    if (baudrate != 0U)
    {
        divisor = ((uint64_t)UART4_CLOCK_HZ + ((uint64_t)baudrate / 2ULL))
            / (uint64_t)baudrate;

        if ((divisor >= (uint64_t)UART_BRR_MIN)
            && (divisor <= (uint64_t)UART_BRR_MAX))
        {
            UART4_InitGpio();
            RCC_MP_APB1ENSETR = RCC_MP_APB1_UART4EN;
            RCC_UART24CKSELR = RCC_UART24CLK_HSI;

            /* Disable UART while changing its format and baud divisor. */
            UART4->CR1 = 0U;
            UART4->CR2 = 0U;
            UART4->CR3 = 0U;
            UART4->BRR = (uint32_t)divisor;
            UART4->ICR = UART_ICR_CLEAR_FLAGS;
            UART4->CR1 = UART_CR1_RE | UART_CR1_TE | UART_CR1_UE;
        }
    }
}

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
)
{
    while ((UART4->ISR & UART_ISR_TXE) == 0U)
    {
        /* Poll the volatile flag; this blocking API has no timeout. */
    }

    UART4->TDR = (uint32_t)_data;
}

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
)
{
    const char *string = _string; /* Local traversal pointer. */

    if (string != NULL)
    {
        while (*string != '\0')
        {
            HAL_UART4_PutChar((uint8_t)*string);
            ++string;
        }
    }
}

/**
****************************************************************************
 * @fn HAL_UART4_GetChar
 * @brief 阻塞等待接收数据，并读取一个字节。
 * @param 无。
 * @return 接收到的字节；调用前需完成 UART 初始化，本函数无超时。
****************************************************************************
 */
uint8_t HAL_UART4_GetChar(void)
{
    uint8_t data; /* 接收字节。 */

    while ((UART4->ISR & UART_ISR_RXNE) == 0U)
    {
        /* Poll the volatile flag; this blocking API has no timeout. */
    }

    data = (uint8_t)(UART4->RDR & UART_DATA_MASK);

    return data;
}

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
)
{
    int32_t status = 0; /* Default for no data or invalid output. */

    if (_data != NULL)
    {
        /* Keep volatile reads out of short-circuit logical expressions. */
        if ((UART4->ISR & UART_ISR_RXNE) != 0U)
        {
            *_data = (uint8_t)(UART4->RDR & UART_DATA_MASK);
            status = 1;
        }
    }

    return status;
}

/**
****************************************************************************
 * @fn HAL_UART4_Flush
 * @brief 等待 UART4 的发送完成标志置位。
 * @param 无。
 * @return 无；调用前需完成 UART 初始化，本函数阻塞且无超时。
****************************************************************************
 */
void HAL_UART4_Flush(void)
{
    while ((UART4->ISR & UART_ISR_TC) == 0U)
    {
        /* The caller must ensure the transmitter can complete. */
    }
}

/* Private function prototypes --------------------------------------------------------------------------*/
/**
****************************************************************************
 * @fn UART4_InitGpio
 * @brief 使能 GPIOB/GPIOG 时钟，并将 PB2/PG11 配置为 UART4 AF6。
 * @param 无。
 * @return 无。
****************************************************************************
 */
static void UART4_InitGpio(void)
{
    const GPIO_INIT_T rx_init =
    {
        GPIO_PIN_2,
        GPIO_AF,
        GPIO_PP,
        GPIO_HIGH_LEVEL,
        GPIO_PU,
        UART4_GPIO_AF
    }; /* PB2 is UART4 RX. */
    const GPIO_INIT_T tx_init =
    {
        GPIO_PIN_11,
        GPIO_AF,
        GPIO_PP,
        GPIO_HIGH_LEVEL,
        GPIO_PU,
        UART4_GPIO_AF
    }; /* PG11 is UART4 TX. */

    RCC_MP_AHB4ENSETR = RCC_MP_AHB4_GPIOBEN | RCC_MP_AHB4_GPIOGEN;
    HAL_GPIO_Init(GPIOB, &rx_init);
    HAL_GPIO_Init(GPIOG, &tx_init);
}

/* uart.c 文件结束。 */
