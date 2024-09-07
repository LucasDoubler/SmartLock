
#include "./SYSTEM/sys/sys.h"


/**
 * @brief       设置中断向量表偏移地址
 * @param       baseaddr: 基址
 * @param       offset: 偏移量
 * @retval      无
 */
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset)
{
    /* 设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留 */
    SCB->VTOR = baseaddr | (offset & (uint32_t)0xFFFFFE00);
}

/**
 * @brief       执行: WFI指令(执行完该指令进入低功耗状态, 等待中断唤醒)
 * @param       无
 * @retval      无
 */
void sys_wfi_set(void)
{
    __ASM volatile("wfi");
}

/**
 * @brief       关闭所有中断(但是不包括fault和NMI中断)
 * @param       无
 * @retval      无
 */
void sys_intx_disable(void)
{
    __ASM volatile("cpsid i");
}

/**
 * @brief       开启所有中断
 * @param       无
 * @retval      无
 */
void sys_intx_enable(void)
{
    __ASM volatile("cpsie i");
}

/**
 * @brief       设置栈顶地址
 * @note        左侧的红X, 属于MDK误报, 实际是没问题的
 * @param       addr: 栈顶地址
 * @retval      无
 */
void sys_msr_msp(uint32_t addr)
{
    __set_MSP(addr);    /* 设置栈顶地址 */
}

/**
 * @brief       使能STM32H7的L1-Cache, 同时开启D cache的强制透写
 * @param       无
 * @retval      无
 */
void sys_cache_enable(void)
{
    SCB_EnableICache(); /* 使能I-Cache,函数在core_cm7.h里面定义 */
    SCB_EnableDCache(); /* 使能D-Cache,函数在core_cm7.h里面定义 */
    SCB->CACR |= 1 << 2;/* 强制D-Cache透写,如不开启透写,实际使用中可能遇到各种问题 */
}

/**
 * @brief       时钟设置函数
 * @param       plln: PLL1倍频系数(PLL倍频), 取值范围: 4~512.
 * @param       pllm: PLL1预分频系数(进PLL之前的分频), 取值范围: 2~63.
 * @param       pllp: PLL1的p分频系数(PLL之后的分频), 分频后作为系统时钟, 取值范围: 2~128.(且必须是2的倍数)
 * @param       pllq: PLL1的q分频系数(PLL之后的分频), 取值范围: 1~128.
 * @note
 *
 *              Fvco: VCO频率
 *              Fsys: 系统时钟频率, 也是PLL1的p分频输出时钟频率
 *              Fq:   PLL1的q分频输出时钟频率
 *              Fs:   PLL输入时钟频率, 可以是HSI, CSI, HSE等.
 *              Fvco = Fs * (plln / pllm);
 *              Fsys = Fvco / pllp = Fs * (plln / (pllm * pllp));
 *              Fq   = Fvco / pllq = Fs * (plln / (pllm * pllq));
 *
 *              外部晶振为25M的时候, 推荐值: plln = 192, pllm = 5, pllp = 2, pllq = 4.
 *              外部晶振为 8M的时候, 推荐值: plln = 240, pllm = 2, pllp = 2, pllq = 4.
 *              得到:Fvco = 25 * (240 / 25) = 240Mhz
 *                   Fsys = pll1_p_ck = 240 / 2 = 120Mhz
 *                   Fq   = pll1_q_ck = 240 / 4 =  60Mhz
 *
 *              H743默认需要配置的频率如下:
 *              CPU频率(rcc_c_ck) = sys_d1cpre_ck = 480Mhz
 *              rcc_aclk = rcc_hclk3 = 240Mhz
 *              AHB1/2/3/4(rcc_hclk1/2/3/4) = 240Mhz
 *              APB1/2/3/4(rcc_pclk1/2/3/4) = 120Mhz
 *              pll2_p_ck = (25 / 25) * 440 / 2) = 220Mhz
 *              pll2_r_ck = FMC时钟频率 = ((25 / 25) * 440 / 2) = 220Mhz
 *
 * @retval      错误代码: 0, 成功; 1, 错误;
 */
uint8_t sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_ClkInitTypeDef rcc_clk_init_handle;
    RCC_OscInitTypeDef rcc_osc_init_handle;
    RCC_PeriphCLKInitTypeDef rcc_periph_clk_init_handle;

    MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);                         /*使能供电配置更新 */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  /* VOS = 1, Scale1, 1.2V内核电压,FLASH访问可以得到最高性能 */
    while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY);     /* 等待电压稳定 */

    /* 使能HSE，并选择HSE作为PLL时钟源，配置PLL1，开启USB时钟 */
    rcc_osc_init_handle.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI48;
    rcc_osc_init_handle.HSEState = RCC_HSE_ON;
    rcc_osc_init_handle.HSIState = RCC_HSI_OFF;
    rcc_osc_init_handle.CSIState = RCC_CSI_OFF;
    rcc_osc_init_handle.HSI48State = RCC_HSI48_ON;
    rcc_osc_init_handle.PLL.PLLState = RCC_PLL_ON;
    rcc_osc_init_handle.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    rcc_osc_init_handle.PLL.PLLN = plln;
    rcc_osc_init_handle.PLL.PLLM = pllm;
    rcc_osc_init_handle.PLL.PLLP = pllp;
    rcc_osc_init_handle.PLL.PLLQ = pllq;
    rcc_osc_init_handle.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    rcc_osc_init_handle.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    rcc_osc_init_handle.PLL.PLLFRACN = 0;
    ret = HAL_RCC_OscConfig(&rcc_osc_init_handle);
    if (ret != HAL_OK)
    {
        return 1;
    }

    /*
     *  选择PLL的输出作为系统时钟
     *  配置RCC_CLOCKTYPE_SYSCLK系统时钟,480M
     *  配置RCC_CLOCKTYPE_HCLK 时钟,240Mhz,对应AHB1，AHB2，AHB3和AHB4总线
     *  配置RCC_CLOCKTYPE_PCLK1时钟,120Mhz,对应APB1总线
     *  配置RCC_CLOCKTYPE_PCLK2时钟,120Mhz,对应APB2总线
     *  配置RCC_CLOCKTYPE_D1PCLK1时钟,120Mhz,对应APB3总线
     *  配置RCC_CLOCKTYPE_D3PCLK1时钟,120Mhz,对应APB4总线
     */

    rcc_clk_init_handle.ClockType = (RCC_CLOCKTYPE_SYSCLK \
                                    | RCC_CLOCKTYPE_HCLK \
                                    | RCC_CLOCKTYPE_PCLK1 \
                                    | RCC_CLOCKTYPE_PCLK2 \
                                    | RCC_CLOCKTYPE_D1PCLK1 \
                                    | RCC_CLOCKTYPE_D3PCLK1);

    rcc_clk_init_handle.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    rcc_clk_init_handle.SYSCLKDivider = RCC_SYSCLK_DIV1;
    rcc_clk_init_handle.AHBCLKDivider = RCC_HCLK_DIV2;
    rcc_clk_init_handle.APB1CLKDivider = RCC_APB1_DIV2; 
    rcc_clk_init_handle.APB2CLKDivider = RCC_APB2_DIV2; 
    rcc_clk_init_handle.APB3CLKDivider = RCC_APB3_DIV2;  
    rcc_clk_init_handle.APB4CLKDivider = RCC_APB4_DIV2; 
    ret = HAL_RCC_ClockConfig(&rcc_clk_init_handle, FLASH_LATENCY_4);
    if (ret != HAL_OK)
    {
        return 1;
    }

    /*
     *  配置PLL2的R分频输出, 为220Mhz
     *  配置FMC时钟源是PLL2R
     *  配置QSPI时钟源是PLL2R
     *  配置串口1 和 串口6 的时钟源来自: PCLK2 = 120Mhz
     *  配置串口2 / 3 / 4 / 5 / 7 / 8 的时钟源来自: PCLK1 = 120Mhz
     *  USB工作需要48MHz的时钟,可以由PLL1Q,PLL3Q和HSI48提供,这里配置时钟源是HSI48
     */
    rcc_periph_clk_init_handle.PeriphClockSelection = RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_USART1
                                                      | RCC_PERIPHCLK_USB| RCC_PERIPHCLK_QSPI
                                                      | RCC_PERIPHCLK_FMC;
    rcc_periph_clk_init_handle.PLL2.PLL2M = 5;
    rcc_periph_clk_init_handle.PLL2.PLL2N = 88;
    rcc_periph_clk_init_handle.PLL2.PLL2P = 2;
    rcc_periph_clk_init_handle.PLL2.PLL2Q = 2;
    rcc_periph_clk_init_handle.PLL2.PLL2R = 2;
    rcc_periph_clk_init_handle.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
    rcc_periph_clk_init_handle.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    rcc_periph_clk_init_handle.PLL2.PLL2FRACN = 0;
    rcc_periph_clk_init_handle.FmcClockSelection = RCC_FMCCLKSOURCE_PLL2;
    rcc_periph_clk_init_handle.QspiClockSelection = RCC_QSPICLKSOURCE_PLL2;
    rcc_periph_clk_init_handle.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    rcc_periph_clk_init_handle.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    rcc_periph_clk_init_handle.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    ret = HAL_RCCEx_PeriphCLKConfig(&rcc_periph_clk_init_handle);
    if (ret != HAL_OK)
    {
        return 1;
    }

    HAL_PWREx_EnableUSBVoltageDetector();   /* 使能USB电压电平检测器 */
    __HAL_RCC_CSI_ENABLE() ;                /* 使能CSI时钟 */
    __HAL_RCC_SYSCFG_CLK_ENABLE() ;         /* 使能SYSCFG时钟 */
    HAL_EnableCompensationCell();           /* 使能IO补偿单元 */
    return 0;
}


#ifdef  USE_FULL_ASSERT

/**
 * @brief       当编译提示出错的时候此函数用来报告错误的文件和所在行
 * @param       file：指向源文件
 *              line：指向在文件中的行数
 * @retval      无
 */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    while (1)
    {
    }
}
#endif




