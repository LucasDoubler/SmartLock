
#include "./SYSTEM/sys/sys.h"


/**
 * @brief       �����ж�������ƫ�Ƶ�ַ
 * @param       baseaddr: ��ַ
 * @param       offset: ƫ����
 * @retval      ��
 */
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset)
{
    /* ����NVIC��������ƫ�ƼĴ���,VTOR��9λ����,��[8:0]���� */
    SCB->VTOR = baseaddr | (offset & (uint32_t)0xFFFFFE00);
}

/**
 * @brief       ִ��: WFIָ��(ִ�����ָ�����͹���״̬, �ȴ��жϻ���)
 * @param       ��
 * @retval      ��
 */
void sys_wfi_set(void)
{
    __ASM volatile("wfi");
}

/**
 * @brief       �ر������ж�(���ǲ�����fault��NMI�ж�)
 * @param       ��
 * @retval      ��
 */
void sys_intx_disable(void)
{
    __ASM volatile("cpsid i");
}

/**
 * @brief       ���������ж�
 * @param       ��
 * @retval      ��
 */
void sys_intx_enable(void)
{
    __ASM volatile("cpsie i");
}

/**
 * @brief       ����ջ����ַ
 * @note        ���ĺ�X, ����MDK��, ʵ����û�����
 * @param       addr: ջ����ַ
 * @retval      ��
 */
void sys_msr_msp(uint32_t addr)
{
    __set_MSP(addr);    /* ����ջ����ַ */
}

/**
 * @brief       ʹ��STM32H7��L1-Cache, ͬʱ����D cache��ǿ��͸д
 * @param       ��
 * @retval      ��
 */
void sys_cache_enable(void)
{
    SCB_EnableICache(); /* ʹ��I-Cache,������core_cm7.h���涨�� */
    SCB_EnableDCache(); /* ʹ��D-Cache,������core_cm7.h���涨�� */
    SCB->CACR |= 1 << 2;/* ǿ��D-Cache͸д,�粻����͸д,ʵ��ʹ���п��������������� */
}

/**
 * @brief       ʱ�����ú���
 * @param       plln: PLL1��Ƶϵ��(PLL��Ƶ), ȡֵ��Χ: 4~512.
 * @param       pllm: PLL1Ԥ��Ƶϵ��(��PLL֮ǰ�ķ�Ƶ), ȡֵ��Χ: 2~63.
 * @param       pllp: PLL1��p��Ƶϵ��(PLL֮��ķ�Ƶ), ��Ƶ����Ϊϵͳʱ��, ȡֵ��Χ: 2~128.(�ұ�����2�ı���)
 * @param       pllq: PLL1��q��Ƶϵ��(PLL֮��ķ�Ƶ), ȡֵ��Χ: 1~128.
 * @note
 *
 *              Fvco: VCOƵ��
 *              Fsys: ϵͳʱ��Ƶ��, Ҳ��PLL1��p��Ƶ���ʱ��Ƶ��
 *              Fq:   PLL1��q��Ƶ���ʱ��Ƶ��
 *              Fs:   PLL����ʱ��Ƶ��, ������HSI, CSI, HSE��.
 *              Fvco = Fs * (plln / pllm);
 *              Fsys = Fvco / pllp = Fs * (plln / (pllm * pllp));
 *              Fq   = Fvco / pllq = Fs * (plln / (pllm * pllq));
 *
 *              �ⲿ����Ϊ25M��ʱ��, �Ƽ�ֵ: plln = 192, pllm = 5, pllp = 2, pllq = 4.
 *              �ⲿ����Ϊ 8M��ʱ��, �Ƽ�ֵ: plln = 240, pllm = 2, pllp = 2, pllq = 4.
 *              �õ�:Fvco = 25 * (240 / 25) = 240Mhz
 *                   Fsys = pll1_p_ck = 240 / 2 = 120Mhz
 *                   Fq   = pll1_q_ck = 240 / 4 =  60Mhz
 *
 *              H743Ĭ����Ҫ���õ�Ƶ������:
 *              CPUƵ��(rcc_c_ck) = sys_d1cpre_ck = 480Mhz
 *              rcc_aclk = rcc_hclk3 = 240Mhz
 *              AHB1/2/3/4(rcc_hclk1/2/3/4) = 240Mhz
 *              APB1/2/3/4(rcc_pclk1/2/3/4) = 120Mhz
 *              pll2_p_ck = (25 / 25) * 440 / 2) = 220Mhz
 *              pll2_r_ck = FMCʱ��Ƶ�� = ((25 / 25) * 440 / 2) = 220Mhz
 *
 * @retval      �������: 0, �ɹ�; 1, ����;
 */
uint8_t sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_ClkInitTypeDef rcc_clk_init_handle;
    RCC_OscInitTypeDef rcc_osc_init_handle;
    RCC_PeriphCLKInitTypeDef rcc_periph_clk_init_handle;

    MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);                         /*ʹ�ܹ������ø��� */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  /* VOS = 1, Scale1, 1.2V�ں˵�ѹ,FLASH���ʿ��Եõ�������� */
    while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY);     /* �ȴ���ѹ�ȶ� */

    /* ʹ��HSE����ѡ��HSE��ΪPLLʱ��Դ������PLL1������USBʱ�� */
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
     *  ѡ��PLL�������Ϊϵͳʱ��
     *  ����RCC_CLOCKTYPE_SYSCLKϵͳʱ��,480M
     *  ����RCC_CLOCKTYPE_HCLK ʱ��,240Mhz,��ӦAHB1��AHB2��AHB3��AHB4����
     *  ����RCC_CLOCKTYPE_PCLK1ʱ��,120Mhz,��ӦAPB1����
     *  ����RCC_CLOCKTYPE_PCLK2ʱ��,120Mhz,��ӦAPB2����
     *  ����RCC_CLOCKTYPE_D1PCLK1ʱ��,120Mhz,��ӦAPB3����
     *  ����RCC_CLOCKTYPE_D3PCLK1ʱ��,120Mhz,��ӦAPB4����
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
     *  ����PLL2��R��Ƶ���, Ϊ220Mhz
     *  ����FMCʱ��Դ��PLL2R
     *  ����QSPIʱ��Դ��PLL2R
     *  ���ô���1 �� ����6 ��ʱ��Դ����: PCLK2 = 120Mhz
     *  ���ô���2 / 3 / 4 / 5 / 7 / 8 ��ʱ��Դ����: PCLK1 = 120Mhz
     *  USB������Ҫ48MHz��ʱ��,������PLL1Q,PLL3Q��HSI48�ṩ,��������ʱ��Դ��HSI48
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

    HAL_PWREx_EnableUSBVoltageDetector();   /* ʹ��USB��ѹ��ƽ����� */
    __HAL_RCC_CSI_ENABLE() ;                /* ʹ��CSIʱ�� */
    __HAL_RCC_SYSCFG_CLK_ENABLE() ;         /* ʹ��SYSCFGʱ�� */
    HAL_EnableCompensationCell();           /* ʹ��IO������Ԫ */
    return 0;
}


#ifdef  USE_FULL_ASSERT

/**
 * @brief       ��������ʾ�����ʱ��˺����������������ļ���������
 * @param       file��ָ��Դ�ļ�
 *              line��ָ�����ļ��е�����
 * @retval      ��
 */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    while (1)
    {
    }
}
#endif




