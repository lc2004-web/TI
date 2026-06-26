#include "Encoder.h"
#include "ti_msp_dl_config.h"
#include "board.h"

static volatile Encoder Encoder_A;  // 左轮
static volatile Encoder Encoder_B;  // 右轮

void Encoder_Init(void)
{
    NVIC_ClearPendingIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOA_INT_IRQn);

    NVIC_ClearPendingIRQ(TIMER_1_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);

    DL_TimerG_startCounter(TIMER_1_INST);

    lc_printf("Encoder initialized (Rising Edge)\r\n");
}

int Motor_Get_Encoder(int n)
{
    if (n == 2) return Encoder_A.Obtained_Get_Encoder_Count;
    else if (n == 1) return Encoder_B.Obtained_Get_Encoder_Count;
    else return 0;
}

// GPIOA 中断处理（上升沿触发）
void GROUP1_IRQHandler(void)
{
    uint32_t gpio_interrupt = DL_GPIO_getEnabledInterruptStatus(Encoder_PORT,
        Encoder_E1A_PIN | Encoder_E1B_PIN | Encoder_E2A_PIN | Encoder_E2B_PIN);

    // 左轮（PA24, PA25）方向逻辑（已根据上升沿调整）
    if (gpio_interrupt & Encoder_E1A_PIN) {
        uint32_t b_state = DL_GPIO_readPins(Encoder_PORT, Encoder_E1B_PIN);
        if ((b_state & Encoder_E1B_PIN) == 0)
            Encoder_A.Should_Get_Encoder_Count--;
        else
            Encoder_A.Should_Get_Encoder_Count++;
    }
    else if (gpio_interrupt & Encoder_E1B_PIN) {
        uint32_t a_state = DL_GPIO_readPins(Encoder_PORT, Encoder_E1A_PIN);
        if ((a_state & Encoder_E1A_PIN) == 0)
            Encoder_A.Should_Get_Encoder_Count++;
        else
            Encoder_A.Should_Get_Encoder_Count--;
    }

    // 右轮（PA26, PA27）方向逻辑
    if (gpio_interrupt & Encoder_E2A_PIN) {
        uint32_t b_state = DL_GPIO_readPins(Encoder_PORT, Encoder_E2B_PIN);
        if ((b_state & Encoder_E2B_PIN) == 0)
            Encoder_B.Should_Get_Encoder_Count++;
        else
            Encoder_B.Should_Get_Encoder_Count--;
    }
    else if (gpio_interrupt & Encoder_E2B_PIN) {
        uint32_t a_state = DL_GPIO_readPins(Encoder_PORT, Encoder_E2A_PIN);
        if ((a_state & Encoder_E2A_PIN) == 0)
            Encoder_B.Should_Get_Encoder_Count--;
        else
            Encoder_B.Should_Get_Encoder_Count++;
    }

    DL_GPIO_clearInterruptStatus(Encoder_PORT,
        Encoder_E1A_PIN | Encoder_E1B_PIN | Encoder_E2A_PIN | Encoder_E2B_PIN);
}

// 定时器中断：更新最终值，并取反使向前转为正数
void TIMG6_IRQHandler(void)
{
    if (DL_TimerG_getPendingInterrupt(TIMER_1_INST) == DL_TIMER_IIDX_ZERO) {
        // 取反，使向前转动输出正值
        Encoder_A.Obtained_Get_Encoder_Count = -Encoder_A.Should_Get_Encoder_Count;
        Encoder_B.Obtained_Get_Encoder_Count = -Encoder_B.Should_Get_Encoder_Count;

        Encoder_A.Should_Get_Encoder_Count = 0;
        Encoder_B.Should_Get_Encoder_Count = 0;

        DL_TimerG_clearInterruptStatus(TIMER_1_INST, DL_TIMER_IIDX_ZERO);
    }
}