#include "uart.h"

/**
 * @brief 初始化 UART4，输出启动提示，并持续回送接收到的字符。
 * @fn main
 * @param 无 本函数无参数。
 * @return 程序退出状态；本程序常驻运行，不会返回。
 */
int main(void)
{
    uint8_t data;

    HAL_UART4_Init(UART4_BAUDRATE);
    HAL_UART4_PutString("UART4 ready (PB2 RX, PG11 TX)\r\n");

    for (;;)
    {
        if (HAL_UART4_TryGetChar(&data))
            HAL_UART4_PutChar(data);
    }
}
