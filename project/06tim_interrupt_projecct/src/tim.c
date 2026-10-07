/**
****************************************************************************
 * @file tim.c
 * @brief PB6 TIM4_CH1 PWM 配置及周期计算。
 * @version 1.3
 * @date 2026-10-05
 * @author Codex
 * @history 1.1: 按仓库编码规范整理。
 * @history 1.2: 改用中文函数注释，移除版权字段，按 Style 1.4 代码示例设置段落。
 * @history 1.3: 增加 TIM4 更新中断初始化和处理函数。
 * @attention Cortex-A7 裸机代码，规范偏离项见 Agent.md。
****************************************************************************
 */

/* Includes ---------------------------------------------------------------------------------------------*/
#include <stddef.h>
#include "gpio.h"
#include "tim.h"

/* Private macro ----------------------------------------------------------------------------------------*/

/* Private enum -----------------------------------------------------------------------------------------*/

/* Private typedef --------------------------------------------------------------------------------------*/
/* Calculation results are committed only after the complete request is valid. */
typedef struct
{
    uint32_t prescaler; /* PSC register value. */
    uint32_t period;    /* ARR register value. */
    uint32_t compare;   /* CCR1 register value. */
    uint32_t mode;      /* OC1M mode, including exact 0% and 100% output. */
} TIM4_PWM_CONFIG_T;

_Static_assert(TIM4_CLOCK_HZ > 0U, "TIM4 clock must be nonzero");
_Static_assert(offsetof(TIM_REG_T, CCR1) == 0x34U, "TIM4 layout mismatch");

/* Exported variables -----------------------------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------------------------------*/
static uint8_t s_ucInitialized; /* Zero until TIM4 has been configured. */

/* Private function declaration -------------------------------------------------------------------------*/
static void TIM4_InitGpio(void);
static int32_t TIM4_CalculatePeriod(
    uint32_t _frequency_hz,        /* PWM 频率，单位 Hz。 */
    uint32_t _duty_percent,        /* 占空比百分数。 */
    TIM4_PWM_CONFIG_T *_config     /* 已校验的寄存器参数。 */
);
static void TIM4_ApplyConfig(
    const TIM4_PWM_CONFIG_T *_config    /* 已校验的寄存器参数。 */
);

/* Exported function declaration ------------------------------------------------------------------------*/
/* 本模块接口已在 tim.h 中声明。 */

/* Exported functions、 prototypes-------------------------------------------------------------------------*/
/**
****************************************************************************
 * @fn HAL_TIM4_PWM_Init
 * @brief 配置 PB6 为 AF2，并初始化和启动 TIM4_CH1 PWM。
 * @param _frequency_hz PWM 频率，单位 Hz，必须可由 16 位定时器表示。
 * @param _duty_percent 占空比百分数，范围为 0～100。
 * @return 成功返回 TIM_PWM_OK；参数非法或频率无法表示时返回 TIM_PWM_ERROR，寄存器不变。
****************************************************************************
 */
int32_t HAL_TIM4_PWM_Init(
    uint32_t _frequency_hz,    /* PWM 频率，单位 Hz。 */
    uint32_t _duty_percent     /* 占空比百分数。 */
)
{
    TIM4_PWM_CONFIG_T config =
    {
        0U, 0U, 0U, 0U
    }; /* Validated settings. */
    int32_t status; /* Initialization result. */

    status = TIM4_CalculatePeriod(_frequency_hz, _duty_percent, &config);

    if (status == TIM_PWM_OK)
    {
        TIM4_InitGpio();
        RCC_MP_APB1ENSETR = RCC_MP_APB1_TIM4EN;

        /* Set registers preserve other peripherals' clock enable states. */
        TIM4->CR1 = 0U;
        TIM4->CR2 = 0U;
        TIM4->SMCR = 0U;
        TIM4->DIER = 0U;
        TIM4->CCER = 0U;
        TIM4_ApplyConfig(&config);
        TIM4->CCER = TIM_CCER_CC1E;
        TIM4->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
        s_ucInitialized = 1U;
    }

    return status;
}

/**
****************************************************************************
 * @fn HAL_TIM4_PWM_Set
 * @brief 修改 PWM 频率和占空比，并保持原有启停状态。
 * @param _frequency_hz PWM 频率，单位 Hz，必须可由 16 位定时器表示。
 * @param _duty_percent 占空比百分数，范围为 0～100。
 * @return 成功返回 TIM_PWM_OK；未初始化或参数非法时返回 TIM_PWM_ERROR，寄存器不变。
****************************************************************************
 */
int32_t HAL_TIM4_PWM_Set(
    uint32_t _frequency_hz,    /* PWM 频率，单位 Hz。 */
    uint32_t _duty_percent     /* 占空比百分数。 */
)
{
    TIM4_PWM_CONFIG_T config =
    {
        0U, 0U, 0U, 0U
    }; /* Validated settings. */
    uint32_t control; /* Saved counter control register. */
    int32_t status = TIM_PWM_ERROR; /* Default for an uninitialized timer. */

    if (s_ucInitialized != 0U)
    {
        status = TIM4_CalculatePeriod(_frequency_hz, _duty_percent, &config);

        if (status == TIM_PWM_OK)
        {
            control = TIM4->CR1;
            TIM4->CR1 = control & ~TIM_CR1_CEN;
            TIM4_ApplyConfig(&config);
            TIM4->CR1 = control;
        }
    }

    return status;
}

/**
****************************************************************************
 * @fn HAL_TIM4_PWM_Enable
 * @brief 使能已初始化的 TIM4_CH1 输出并启动计数器。
 * @param 无。
 * @return 无；未初始化时不执行操作。
****************************************************************************
 */
void HAL_TIM4_PWM_Enable(void)
{
    if (s_ucInitialized != 0U)
    {
        TIM4->CCER |= TIM_CCER_CC1E;
        TIM4->CR1 |= TIM_CR1_CEN;
    }
}

/**
****************************************************************************
 * @fn HAL_TIM4_PWM_Disable
 * @brief 关闭已初始化的 TIM4_CH1 输出并停止计数器。
 * @param 无。
 * @return 无；未初始化时不执行操作。
****************************************************************************
 */
void HAL_TIM4_PWM_Disable(void)
{
    if (s_ucInitialized != 0U)
    {
        TIM4->CCER &= ~TIM_CCER_CC1E;
        TIM4->CR1 &= ~TIM_CR1_CEN;
    }
}

/**
****************************************************************************
 * @fn HAL_TIM4_InterruptInit
 * @brief 初始化 TIM4 更新中断，并以指定频率启动定时器。
 * @param _frequency_hz 更新中断频率，单位 Hz，必须可由 16 位计数器表示。
 * @return 成功返回 0；参数非法或频率无法表示时返回 -1。
****************************************************************************
 */
int32_t HAL_TIM4_InterruptInit(
    uint32_t _frequency_hz    /* 更新中断频率。 */
)
{
    uint64_t denominator;
    uint64_t divider;
    uint64_t ticks;
    uint32_t prescaler;
    uint32_t period;
    const GPIO_INIT_T led_init =
    {
        GPIO_PIN_8,
        GPIO_OUTPUT,
        GPIO_PP,
        GPIO_LOW_LEVEL,
        GPIO_NOPUPD,
        0U
    };
    int32_t status = -1;

    if (_frequency_hz != 0U)
    {
        denominator = (uint64_t)_frequency_hz * (uint64_t)TIM_COUNTER_STATES;
        divider = ((uint64_t)TIM4_CLOCK_HZ + denominator - 1ULL)
            / denominator;

        if (divider == 0ULL)
        {
            divider = 1ULL;
        }

        if (divider <= (uint64_t)TIM_COUNTER_STATES)
        {
            ticks = (uint64_t)TIM4_CLOCK_HZ
                / (divider * (uint64_t)_frequency_hz);

            if ((ticks >= 2ULL) && (ticks <= (uint64_t)TIM_COUNTER_STATES))
            {
                prescaler = (uint32_t)divider - 1U;
                period = (uint32_t)ticks - 1U;

                RCC_MP_AHB4ENSETR |= RCC_MP_AHB4_GPIOFEN;
                HAL_GPIO_Init(GPIOF, &led_init);
                HAL_GPIO_Write(GPIOF, GPIO_PIN_8, GPIO_RESET);
                RCC_MP_APB1ENSETR |= RCC_MP_APB1_TIM4EN;

                TIM4->CR1 = 0U;
                TIM4->DIER = 0U;
                TIM4->PSC = prescaler;
                TIM4->ARR = period;
                TIM4->EGR = TIM_EGR_UG;
                TIM4->SR = 0U;
                TIM4->DIER = TIM_DIER_UIE;
                TIM4->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
                status = 0;
            }
        }
    }

    return status;
}

/**
****************************************************************************
 * @fn HAL_TIM4_IRQHandler
 * @brief 清除 TIM4 更新标志，并反转 PF8 LED。
 * @param 无。
 * @return 无；没有更新中断 pending 时不执行 LED 操作。
****************************************************************************
 */
void HAL_TIM4_IRQHandler(void)
{
    if ((TIM4->SR & TIM_SR_UIF) != 0U)
    {
        TIM4->SR &= ~TIM_SR_UIF;
        HAL_GPIO_Toggle(GPIOF, GPIO_PIN_8);
    }
}

/* Private function prototypes --------------------------------------------------------------------------*/
/**
****************************************************************************
 * @fn TIM4_InitGpio
 * @brief 使能 GPIOB 时钟，并将 PB6 配置为 TIM4_CH1 AF2。
 * @param 无。
 * @return 无。
****************************************************************************
 */
static void TIM4_InitGpio(void)
{
    const GPIO_INIT_T init =
    {
        GPIO_PIN_6,      /* Buzzer signal. */
        GPIO_AF,         /* Timer output. */
        GPIO_PP,         /* Push-pull driver. */
        GPIO_HIGH_LEVEL, /* Output slew rate. */
        GPIO_NOPUPD,     /* No internal pull resistor. */
        TIM4_GPIO_AF     /* TIM4_CH1 alternate function. */
    };

    RCC_MP_AHB4ENSETR = RCC_MP_AHB4_GPIOBEN;
    HAL_GPIO_Init(GPIOB, &init);
}

/**
****************************************************************************
 * @fn TIM4_CalculatePeriod
 * @brief 计算 16 位预分频值、周期值、比较值及输出模式。
 * @param _frequency_hz PWM 频率，单位 Hz，必须可由 16 位定时器表示。
 * @param _duty_percent 占空比百分数，范围为 0～100。
 * @param _config 计算结果输出指针，不能为 NULL。
 * @return 成功返回 TIM_PWM_OK；失败返回 TIM_PWM_ERROR，输出结构不变。
****************************************************************************
 */
static int32_t TIM4_CalculatePeriod(
    uint32_t _frequency_hz,       /* PWM 频率，单位 Hz。 */
    uint32_t _duty_percent,       /* 占空比百分数。 */
    TIM4_PWM_CONFIG_T *_config    /* 已校验的寄存器参数。 */
)
{
    uint64_t denominator; /* Wide frequency product prevents overflow. */
    uint64_t divider;     /* Prescaler divisor, 1..65536. */
    uint64_t ticks;       /* Counter ticks per PWM period, 2..65536. */
    uint32_t mode = TIM_CCMR1_OC1M_PWM1; /* Normal PWM output mode. */
    int32_t status = TIM_PWM_ERROR; /* Default for invalid input. */

    if ((_frequency_hz != 0U) && (_duty_percent <= TIM_DUTY_MAX)
        && (_config != NULL))
    {
        denominator = (uint64_t)_frequency_hz * (uint64_t)TIM_COUNTER_STATES;
        divider = ((uint64_t)TIM4_CLOCK_HZ + denominator - 1ULL) / denominator;

        if (divider == 0ULL)
        {
            divider = 1ULL;
        }

        if (divider <= (uint64_t)TIM_COUNTER_STATES)
        {
            ticks = (uint64_t)TIM4_CLOCK_HZ
                / (divider * (uint64_t)_frequency_hz);

            if ((ticks >= 2ULL) && (ticks <= (uint64_t)TIM_COUNTER_STATES))
            {
                /* Forced levels avoid a CCR1 overflow at exactly 100% duty. */
                if (_duty_percent == 0U)
                {
                    mode = TIM_CCMR1_OC1M_INACTIVE;
                }
                else if (_duty_percent == TIM_DUTY_MAX)
                {
                    mode = TIM_CCMR1_OC1M_ACTIVE;
                }
                else
                {
                    /* Intermediate duty cycles use PWM mode 1. */
                }

                _config->prescaler = (uint32_t)divider - 1U;
                _config->period = (uint32_t)ticks - 1U;
                _config->mode = mode;
                _config->compare = 0U;

                if (mode == TIM_CCMR1_OC1M_PWM1)
                {
                    _config->compare = (uint32_t)((ticks * (uint64_t)_duty_percent)
                        / (uint64_t)TIM_DUTY_MAX);
                }

                status = TIM_PWM_OK;
            }
        }
    }

    return status;
}

/**
****************************************************************************
 * @fn TIM4_ApplyConfig
 * @brief 写入已经校验的定时器参数，并产生更新事件。
 * @param _config 已校验的参数指针；调用前需停止定时器，NULL 不执行操作。
 * @return 无。
****************************************************************************
 */
static void TIM4_ApplyConfig(
    const TIM4_PWM_CONFIG_T *_config    /* 已校验的寄存器参数。 */
)
{
    if (_config != NULL)
    {
        TIM4->CCMR1 = TIM_CCMR1_OC1PE | _config->mode;
        TIM4->PSC = _config->prescaler;
        TIM4->ARR = _config->period;
        TIM4->CCR1 = _config->compare;
        TIM4->EGR = TIM_EGR_UG;
        TIM4->SR = 0U;
    }
}

/* tim.c 文件结束。 */
