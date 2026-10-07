# STM32MP157 裸机外设学习工程

本仓库当前包含四个可运行的 Cortex-A7 裸机工程：`03project` 用于 UART4 收发，`04pwm_project` 用于 TIM4_CH1 蜂鸣器 PWM，`05interrupt_project` 用于 PF9 按键下降沿中断反转 PF8 LED，`06tim_interrupt_projecct` 用于 TIM4 更新中断。工程都通过 U-Boot 的 `loadb` 下载到 DDR，再由 `go` 命令跳转执行。

## 工程索引

| 工程 | 功能 | 输出文件 | 外设引脚 |
| --- | --- | --- | --- |
| `project/03project` | UART4 字符回显 | `build/uart4.bin` | PB2 RX、PG11 TX |
| `project/04pwm_project` | TIM4_CH1 蜂鸣器 PWM | `build/buzzer.bin` | PB6，AF2 |
| `project/05interrupt_project` | PF9 下降沿中断反转 PF8 LED | `build/interrupt.bin` | PF9 按键、PF8 LED |
| `project/06tim_interrupt_projecct` | TIM4 更新中断反转 PF8 LED | `build/interrupt.bin` | PF8 LED |

两个工程默认使用 `0xc0008000` 作为链接地址、下载地址和执行地址，编译参数为 Cortex-A7 ARM 状态代码。

## 03project：UART4

```text
project/03project/
├── main.c          # UART4 初始化、启动提示和字符回显
├── startup.S       # Cortex-A7 ARM 状态启动代码
├── map.lds         # DDR 链接脚本，入口地址为 0xc0008000
├── Makefile        # ARM 交叉编译和 bin 文件生成规则
├── inc/gpio.h      # GPIO 寄存器和 GPIO 复用配置
├── inc/uart.h      # UART4 寄存器、时钟和 API 定义
├── src/gpio.c      # GPIO 初始化及 AF 配置实现
└── src/uart.c      # UART4 驱动实现
```

## 硬件连接

UART4 使用以下引脚：

| UART4 信号 | GPIO 引脚 | 复用功能 |
| --- | --- | --- |
| UART4_RX | PB2 | AF6 |
| UART4_TX | PG11 | AF6 |

连接 USB 转串口时，开发板 `PG11` 接转换器 RX，开发板 `PB2` 接转换器 TX，同时连接 GND。串口终端配置为 `115200 8-N-1`。

## 程序行为

程序启动后初始化 UART4，并发送：

```text
UART4 ready (PB2 RX, PG11 TX)
```

之后进入轮询接收模式，将收到的每个字符原样发送回串口终端。

默认时钟和波特率定义在 `inc/uart.h`：

```c
#define UART4_CLOCK_HZ 64000000U
#define UART4_BAUDRATE 115200U
```

UART4 内核时钟选择为 HSI 64 MHz。若平台启动代码改变了 UART4 内核时钟，需要同步修改 `UART4_CLOCK_HZ`。

## 编译环境

工程需要支持 Cortex-A7 的 ARM 交叉编译器。默认工具链前缀为 `arm-none-eabi-`，也可以通过 `CROSS_COMPILE` 覆盖：

```sh
export CROSS_COMPILE=arm-none-eabi-
# 也可以使用已安装的其他工具链，例如：
# export CROSS_COMPILE=arm-linux-gnueabihf-
```

编译选项使用 `-mcpu=cortex-a7 -marm`，因此生成的是 ARM 状态代码，适合 U-Boot 的 `go 0xc0008000` 入口方式。

## 编译

在工程目录执行：

```sh
cd project/03project
make CROSS_COMPILE=arm-none-eabi-
```

成功后生成：

```text
project/03project/build/uart4.elf
project/03project/build/uart4.bin
```

其中 `uart4.bin` 是通过 `loadb` 下载的文件；`uart4.elf` 用于调试和查看符号、段地址。

常用清理命令：

```sh
make clean
```

## U-Boot 下载和运行

1. 启动开发板进入 U-Boot 命令行。
2. 执行以下命令，等待 U-Boot 开始接收文件：

   ```text
   loadb 0xc0008000
   ```

3. 在终端工具中使用 `loadb` 对应的 Kermit 协议发送 `build/uart4.bin`。
4. 文件传输完成并返回 U-Boot 后执行：

   ```text
   go 0xc0008000
   ```

5. 使用连接到 UART4 的串口终端观察启动信息并发送字符测试回显。

链接脚本已经将 `.text` 放在 `0xc0008000`，所以下载地址和执行地址必须保持一致。该 A7 工程不使用 Cortex-M 的中断向量表。

## 常见问题

### 没有生成 `uart4.bin`

检查 `CROSS_COMPILE` 指向的编译器是否存在，并确认 `make` 在 `project/03project` 目录执行。

### 能执行但没有串口输出

检查 USB 转串口的 TX/RX 是否交叉连接、GND 是否连接，以及终端是否设置为 `115200 8-N-1`。同时确认板卡实际将 UART4 引出到 PB2 和 PG11。

### 下载后 `go` 无法运行

确认文件是 `build/uart4.bin`，下载地址是 `0xc0008000`，执行命令也是 `go 0xc0008000`，并且编译器生成的是 ARM 状态代码。

## 04pwm_project：TIM4_CH1 蜂鸣器

该工程使用 PB6 的 `TIM4_CH1` 复用功能驱动蜂鸣器：PB6 配置为 AF2，TIM4 输出 PWM。默认参数为 2 kHz、50% 占空比，适合常见无源蜂鸣器。PWM 输入时钟默认按 64 MHz 计算，定义在 `project/04pwm_project/inc/tim.h` 的 `TIM4_CLOCK_HZ`；如果板级时钟树不同，需要修改该宏。

工程目录为 `project/04pwm_project`，主要文件如下：

- `inc/tim.h`：TIM4 寄存器定义和 PWM API。
- `inc/rcc.h`：GPIO、TIM4 和保留 UART 驱动使用的公共 RCC 寄存器及使能位。
- `src/tim.c`：PB6 复用配置、定时器分频、周期和占空比计算。
- `main.c`：启动 2 kHz/50% PWM 并保持运行。
- `Makefile`、`map.lds`、`startup.S`：A7 构建和 `0xc0008000` 裸机启动配置。

`inc/uart.h` 和 `src/uart.c` 是从前一个工程保留的 UART 驱动文件，当前 Makefile 不会编译它们；蜂鸣器程序只使用 GPIO、TIM4 和启动文件。

编译：

```sh
cd project/04pwm_project
make CROSS_COMPILE=arm-none-eabi-
```

生成文件为 `build/buzzer.bin`。在 U-Boot 中使用以下命令下载和运行：

```text
loadb 0xc0008000
go 0xc0008000
```

`loadb` 等待文件时，通过终端工具发送 `build/buzzer.bin`。蜂鸣器应连接到 PB6 对应的输出端，并根据硬件电路确认是否需要反相驱动或外部三极管。软件 API `HAL_TIM4_PWM_Set(frequency_hz, duty_percent)` 可在运行期间修改音调和占空比，`HAL_TIM4_PWM_Enable()`、`HAL_TIM4_PWM_Disable()` 可控制输出。

### PWM 接口约定

- `HAL_TIM4_PWM_Init()` 和 `HAL_TIM4_PWM_Set()` 返回 `int32_t`；成功为 `TIM_PWM_OK`（0），失败为 `TIM_PWM_ERROR`（-1）。调用方必须检查返回值。
- 频率必须非零且能够由 16 位预分频器和计数器表示，占空比范围为 0～100。非法请求不会更新外设寄存器。
- 必须先成功初始化，再调用 Set/Enable/Disable。初始化之前 Set 返回失败，Enable/Disable 不访问 TIM4。
- Set 保持计数器原来的启停状态。0% 和 100% 分别使用强制无效/有效输出模式，避免 100% 时 CCR1 超过 16 位。
- 实际频率和中间占空比受整数分频精度影响；Disable 停止计数并关闭通道，不保证外部蜂鸣器电路的关断电平。
- GPIO 寄存器类型改为 `GPIO_REG_T`，GPIO 状态枚举改为 `GPIO_STATE_E`；TIM/UART 寄存器类型分别为 `TIM_REG_T`、`UART_REG_T`。引用旧类型的调用代码需要同步更新。

## 05interrupt_project：PF9 按键中断

该工程使用 PF9 作为按键输入，配置为上拉输入和下降沿 EXTI9 中断；按键按下产生中断后，IRQ 处理函数反转 PF8 的 LED 输出电平。

工程主要文件如下：

- inc/exti.h、src/exti.c：PF9 的 SYSCFG 端口映射、EXTI9 触发配置、pending 清除和 PF8 LED 反转。
- inc/gic.h、src/gic.c：Cortex-A7 GIC-400 初始化、SPI 23 使能和 IRQ 分发。
- startup.S：A7 异常向量表、IRQ 模式栈、IRQ 入口保存与恢复现场。
- main.c：初始化 EXTI/GIC，打开 IRQ 后使用 wfi 等待按键。
- inc/gpio.h、src/gpio.c：GPIOF 配置和 PF8 输出反转接口。

PF9 的 EXTI9 属于 STM32MP157 的 EXTI5～9 共享中断，映射到 GIC SPI 23，并按电平触发方式配置 GIC。PF9 使用内部上拉，因此按键电路应在按下时将 PF9 拉低；如果实际电路为高电平按下，需要调整输入上下拉和触发边沿。

编译：

    cd project/05interrupt_project
    make CROSS_COMPILE=arm-none-eabi-

生成 build/interrupt.bin 后，在 U-Boot 中执行 loadb 0xc0008000，传输完成后执行 go 0xc0008000。

启动代码的向量表位于镜像首地址，Makefile 将 ELF 入口设置为 vector_table，因此 go 0xc0008000 会先进入向量表再跳转到 _start。当前实现假定 U-Boot 已完成 DDR、时钟和 GIC 所需的基础安全/电源配置；尚未进行开发板按键和 LED 电气实测。

## 06tim_interrupt_projecct：TIM4 更新中断

该工程使用 TIM4 基本计数器更新事件产生中断，默认中断频率为 1 Hz。每次 TIM4 更新中断进入 IRQ 后，`HAL_TIM4_IRQHandler` 清除 UIF 标志并反转 PF8 LED。TIM4 的 GIC 中断号配置为 SPI 30，启动代码复用 05 工程的 A7 向量表和 IRQ 模式栈。

工程主要文件如下：

- `inc/tim.h`、`src/tim.c`：TIM4 预分频和 ARR 计算、更新中断使能及中断处理函数。
- `inc/gic.h`、`src/gic.c`：GIC SPI 30 初始化、CPU interface 配置和 TIM4 IRQ 分发。
- `main.c`：初始化 TIM4 和 GIC，打开 IRQ 后执行 `wfi` 等待中断。
- `startup.S`：A7 异常向量表和 IRQ 现场保存/恢复。

编译：

    cd project/06tim_interrupt_projecct
    make CROSS_COMPILE=arm-none-eabi-

生成 `build/interrupt.bin` 后，在 U-Boot 中执行 `loadb 0xc0008000`，传输完成后执行 `go 0xc0008000`。修改 `inc/tim.h` 中的 `TIM4_INTERRUPT_FREQUENCY_HZ` 可改变中断频率，但实际频率受 64 MHz 时钟和 16 位 PSC/ARR 整数分频限制。

该目录中保留了从 05 工程复制来的 EXTI 文件，但默认 Makefile 不编译这些文件；当前工程只启用 TIM4 更新中断。尚未进行开发板实测，板上应确认 TIM4 时钟树实际为 64 MHz、GIC SPI 30 映射正确，并观察 PF8 LED 是否按设定频率反转。

## 编码规范与维护

规范来源：`编码规范/SLC 27 Pro_Software coding specification_V0.1_20260423.xlsx`。
本次整理范围包括最近的 `project/04pwm_project`、`project/05interrupt_project` 和 `project/06tim_interrupt_projecct`；03 工程尚未按此规范全面整理。

### 已落实的规则

- 按用户约定，函数注释使用中文，源文件采用 UTF-8 编码，统一使用块注释。函数注释包含 `@fn`、`@brief`、`@param`、`@return`，无参数或不返回的情况明确说明。
- 文件增加用途、版本、日期、修改记录和注意事项；按用户约定不写 Style 3.4 中的文件版权字段。
- 段落布局采用 Style 1.4 的代码示例，使用示例中的英文横线分隔标题和排列顺序。C 文件使用 `Includes`、`Private macro`、`Private enum`、`Private typedef`、`Exported variables`、`Private variables`、`Private function declaration`、`Exported function declaration`、`Exported functions、 prototypes`、`Private function prototypes`；头文件使用 `Define to prevent recursive inclusion`、`Includes`、`Exported macros`、`Exported enum`、`Exported types`、`Exported variables`、`Exported functions`。空段落保留示例标题，标题不添加描述中的字母编号或“无”。C 文件公共驱动接口通过对应头文件声明，实现顺序与头文件一致。
- C 缩进为四个空格，大括号单独成行，条件和循环使用完整代码块；函数参数单独成行并加说明。
- 形参使用 `_lower_snake_case`，局部变量使用小写名称，文件静态变量使用 `s_` 加类型前缀，结构体使用 `_T`，枚举使用 `_E`；头文件保护宏不使用保留的双下划线名称。
- 使用定宽整数、无符号位运算、明确的条件判断、NULL 检查和参数范围校验；有返回值的驱动函数采用单一返回出口。
- PWM 参数先验证再写寄存器，主程序检查初始化结果。GPIO 操作要求端口已开启时钟，ODR 的读改写要求调用方独占端口。
- Makefile 启用类型转换、符号转换、遮蔽和函数原型警告，并将警告视为错误；通过依赖文件在头文件变化后重新编译。
- 链接脚本明确放置 ARM 展开表，DDR 中 data 的加载地址与运行地址一致，并预留至少 4 KiB 栈空间。

### 裸机代码偏离与适用限制

以下内容需要在正式 MISRA/IEC 评审中确认，不能仅凭本次修改认定完全合规：

| 条目 | 当前处理与原因 |
| --- | --- |
| 整数转换为寄存器指针（MISRA 11.4） | 固定 MMIO 地址转换集中于寄存器头文件，要求 Cortex-A7 外设地址可访问且内存属性正确。寄存器指针必须由调用方保证有效，NULL 检查无法证明任意指针有效。 |
| 编译器标准头文件 | 使用 `stdint.h`、`stddef.h` 定义定宽类型、NULL 和布局检查；不调用 C 标准库运行时函数。 |
| “仅静态变量”要求（IEC 10.10） | 无动态内存分配；保留有界的自动局部变量和临时配置结构，避免全部静态化导致共享状态与不可重入。若该条按静态存储期严格执行，需要进一步评审此偏离。 |
| 编译器扩展和支持库 | 使用 C11 `_Static_assert` 检查寄存器布局，64 位除法依赖工具链 `libgcc`；GCC 构建尚未提供安全认证证据。 |
| 启动汇编与文件名 | ARM 启动代码隔离于 `startup.S`，保留大写后缀以启用预处理。链接脚本符号和入口 `_start` 按工具链约定保留。 |
| 硬件寄存器成员名 | MODER、CR1 等名称按芯片寄存器文档保留大写；结构布局不可随意调整。 |
| 常驻循环和阻塞轮询 | main 和启动入口不返回；保留的 UART 阻塞接口没有超时，依赖正确初始化及有效时钟，不适用于直接宣称具备故障恢复能力。 |
| 时钟与执行环境 | 依赖 U-Boot 已完成 DDR 和平台初始化。TIM4 时钟默认假定为 64 MHz；保留 UART 驱动选择 HSI，要求 HSI 已开启且其实际频率与宏一致。 |

保留的 04 UART 驱动还修正了 UE 为 bit 0，并使用限定的 ICR 清除位，避免写入全部保留位。这些改动不影响当前默认蜂鸣器构建，03 工程的 UART 驱动仍需另行检查。

### 本次验证

在开发主机完成以下检查：

- `make -B` 使用 `arm-none-eabi-gcc` 构建通过，生成 `build/buzzer.elf` 和 `build/buzzer.bin`；保留的 UART 源文件单独通过相同严格警告检查。
- 寄存器模拟测试通过，并启用未定义行为检测：覆盖 GPIO 第 15 引脚、空指针/非法枚举、PWM 默认参数、非法参数不写寄存器、频率边界、0%/100% 占空比及关闭后 Set 保持关闭。
- UART 模拟检查覆盖默认波特率、非法波特率、发送与接收；模拟不能验证实际串口时序、时钟源或电气连接。
- ELF 入口确认为 `0xc0008000`，代码为 ARM 状态，下载运行方式仍为 `loadb 0xc0008000` 后 `go 0xc0008000`。

尚未完成开发板实测、完整 MISRA 静态分析或 IEC 61508 安全评估；板上应使用示波器确认 PB6 的实际频率、占空比及蜂鸣器关断电平。

## 相关资料

- `STM32MP157手册/`：STM32MP157 数据手册和参考手册。
- `原理图/`：FS-MP1A 主板及扩展板原理图。
- `project/03project/inc/uart.h`：UART4 基地址、RCC 地址和寄存器位定义。
- `project/04pwm_project/inc/tim.h`：TIM4 基地址、RCC 地址和 PWM API 定义。
