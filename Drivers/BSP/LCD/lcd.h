

#ifndef __LCD_H
#define __LCD_H

#include "stdlib.h"
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* LCD RST/WR/RD/BL/CS/RS 引脚 定义 
 * LCD_D0~D15,由于引脚太多,就不在这里定义了,直接在lcd_init里面修改.所以在移植的时候,除了改
 * 这6个IO口, 还得改LCD_Init里面的D0~D15所在的IO口.
 */

/* RESET 和系统复位脚共用 所以这里不用定义 RESET引脚 */
//#define LCD_RST_GPIO_PORT               GPIOx
//#define LCD_RST_GPIO_PIN                SYS_GPIO_PINx
//#define LCD_RST_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOx_CLK_ENABLE(); }while(0)   /* 所在IO口时钟使能 */

#define LCD_WR_GPIO_PORT                GPIOD
#define LCD_WR_GPIO_PIN                 GPIO_PIN_5
#define LCD_WR_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* 所在IO口时钟使能 */

#define LCD_RD_GPIO_PORT                GPIOD
#define LCD_RD_GPIO_PIN                 GPIO_PIN_4
#define LCD_RD_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* 所在IO口时钟使能 */

#define LCD_BL_GPIO_PORT                GPIOB
#define LCD_BL_GPIO_PIN                 GPIO_PIN_5
#define LCD_BL_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)     /* 背光所在IO口时钟使能 */

/* LCD_CS(需要根据LCD_FMC_NEX设置正确的IO口) 和 LCD_RS(需要根据LCD_FMC_AX设置正确的IO口) 引脚 定义 */
#define LCD_CS_GPIO_PORT                GPIOD
#define LCD_CS_GPIO_PIN                 GPIO_PIN_7
#define LCD_CS_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* 所在IO口时钟使能 */

#define LCD_RS_GPIO_PORT                GPIOD
#define LCD_RS_GPIO_PIN                 GPIO_PIN_13
#define LCD_RS_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* 所在IO口时钟使能 */

/* FMC相关参数 定义 
 * 注意: 我们默认是通过FMC块1来连接LCD, 块1有4个片选: FMC_NE1~4
 *
 * 修改LCD_FMC_NEX, 对应的LCD_CS_GPIO相关设置也得改
 * 修改LCD_FMC_AX , 对应的LCD_RS_GPIO相关设置也得改
 */
#define LCD_FMC_NEX         1       /* 使用FMC_NE1接LCD_CS,取值范围只能是: 1~4 */
#define LCD_FMC_AX          18      /* 使用FMC_A18接LCD_RS,取值范围是: 0 ~ 25 */

/******************************************************************************************/

/* LCD重要参数集 */
typedef struct
{
    uint16_t width;                 /* LCD 宽度 */
    uint16_t height;                /* LCD 高度 */
    uint16_t id;                    /* LCD ID */
    uint8_t dir;                    /* 横屏还是竖屏控制：0，竖屏；1，横屏。 */
    uint16_t wramcmd;               /* 开始写gram指令 */
    uint16_t setxcmd;               /* 设置x坐标指令 */
    uint16_t setycmd;               /* 设置y坐标指令 */
} _lcd_dev;

/* LCD参数 */
extern _lcd_dev lcddev;             /* 管理LCD重要参数 */

/* LCD的画笔颜色和背景色 */
extern uint32_t  g_point_color;     /* 默认红色 */
extern uint32_t  g_back_color;      /* 背景颜色.默认为白色 */

/* LCD背光控制 */
#define LCD_BL(x)   do{ x ? \
                      HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_RESET); \
                     }while(0)

/* LCD地址结构体 */
typedef struct
{
    volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
} LCD_TypeDef;

/* LCD MPU保护参数 */
#define LCD_REGION_NUMBER       MPU_REGION_NUMBER0      /* LCD使用region0 */
#define LCD_ADDRESS_START       (0X60000000)            /* LCD区的首地址 */
#define LCD_REGION_SIZE         MPU_REGION_SIZE_256MB   /* LCD区大小 */

/* LCD_BASE的详细解算方法:
 * 我们一般使用FMC的块1(BANK1)来驱动TFTLCD液晶屏(MCU屏), 块1地址范围总大小为256MB,均分成4块:
 * 存储块1(FMC_NE1)地址范围: 0x6000 0000 ~ 0x63FF FFFF
 * 存储块2(FMC_NE2)地址范围: 0x6400 0000 ~ 0x67FF FFFF
 * 存储块3(FMC_NE3)地址范围: 0x6800 0000 ~ 0x6BFF FFFF
 * 存储块4(FMC_NE4)地址范围: 0x6C00 0000 ~ 0x6FFF FFFF
 *
 * 我们需要根据硬件连接方式选择合适的片选(连接LCD_CS)和地址线(连接LCD_RS)
 * 阿波罗H743开发板使用FMC_NE1连接LCD_CS, FMC_A18连接LCD_RS ,16位数据线,计算方法如下:
 * 首先FMC_NE1的基地址为: 0x6000 0000;     NEX的基址为(x=1/2/3/4): 0x6000 0000 + (0x400 0000 * (x - 1))
 * FMC_A18对应地址值: 2^18 * 2 = 0x80000;    FMC_Ay对应的地址为(y = 0 ~ 25): 2^y * 2
 *
 * LCD->LCD_REG,对应LCD_RS = 0(LCD寄存器); LCD->LCD_RAM,对应LCD_RS = 1(LCD数据)
 * 则 LCD->LCD_RAM的地址为:  0x6000 0000 + 2^18 * 2 = 0x6008 0000
 *    LCD->LCD_REG的地址可以为 LCD->LCD_RAM之外的任意地址.
 * 由于我们使用结构体管理LCD_REG 和 LCD_RAM(REG在前,RAM在后,均为16位数据宽度)
 * 因此 结构体的基地址(LCD_BASE) = LCD_RAM - 2 = 0x6008 0000 -2
 *
 * 更加通用的计算公式为((片选脚FSMC_NEX)X=1/2/3/4, (RS接地址线FSMC_Ay)y=0~25):
 *          LCD_BASE = (0x6000 0000 + (0x400 0000 * (x - 1))) | (2^y * 2 -2)
 *          等效于(使用移位操作)
 *          LCD_BASE = (0x6000 0000 + (0x400 0000 * (x - 1))) | ((1 << y) * 2 -2)
 */
#define LCD_BASE        (uint32_t)((0X60000000 + (0X4000000 * (LCD_FMC_NEX - 1))) | (((1 << LCD_FMC_AX) * 2) - 2))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

/******************************************************************************************/
/* LCD扫描方向和颜色 定义 */

/* 扫描方向定义 */
#define L2R_U2D         0           /* 从左到右,从上到下 */
#define L2R_D2U         1           /* 从左到右,从下到上 */
#define R2L_U2D         2           /* 从右到左,从上到下 */
#define R2L_D2U         3           /* 从右到左,从下到上 */

#define U2D_L2R         4           /* 从上到下,从左到右 */
#define U2D_R2L         5           /* 从上到下,从右到左 */
#define D2U_L2R         6           /* 从下到上,从左到右 */
#define D2U_R2L         7           /* 从下到上,从右到左 */

#define DFT_SCAN_DIR    L2R_U2D     /* 默认的扫描方向 */

/* 常用画笔颜色 */
#define WHITE           0xFFFF      /* 白色 */
#define BLACK           0x0000      /* 黑色 */
#define RED             0xF800      /* 红色 */
#define GREEN           0x07E0      /* 绿色 */
#define BLUE            0x001F      /* 蓝色 */ 
#define MAGENTA         0xF81F      /* 品红色/紫红色 = BLUE + RED */
#define YELLOW          0xFFE0      /* 黄色 = GREEN + RED */
#define CYAN            0x07FF      /* 青色 = GREEN + BLUE */  

/* 非常用颜色 */
#define BROWN           0xBC40      /* 棕色 */
#define BRRED           0xFC07      /* 棕红色 */
#define GRAY            0x8430      /* 灰色 */ 
#define DARKBLUE        0x01CF      /* 深蓝色 */
#define LIGHTBLUE       0x7D7C      /* 浅蓝色 */ 
#define GRAYBLUE        0x5458      /* 灰蓝色 */ 
#define LIGHTGREEN      0x841F      /* 浅绿色 */  
#define LGRAY           0xC618      /* 浅灰色(PANNEL),窗体背景色 */ 
#define LGRAYBLUE       0xA651      /* 浅灰蓝色(中间层颜色) */ 
#define LBBLUE          0x2B12      /* 浅棕蓝色(选择条目的反色) */ 

/******************************************************************************************/
/* SSD1963相关配置参数(一般不用改) */

/* LCD分辨率设置 */ 
#define SSD_HOR_RESOLUTION      800     /* LCD水平分辨率 */ 
#define SSD_VER_RESOLUTION      480     /* LCD垂直分辨率 */ 

/* LCD驱动参数设置 */ 
#define SSD_HOR_PULSE_WIDTH     1       /* 水平脉宽 */ 
#define SSD_HOR_BACK_PORCH      46      /* 水平前廊 */ 
#define SSD_HOR_FRONT_PORCH     210     /* 水平后廊 */ 

#define SSD_VER_PULSE_WIDTH     1       /* 垂直脉宽 */ 
#define SSD_VER_BACK_PORCH      23      /* 垂直前廊 */ 
#define SSD_VER_FRONT_PORCH     22      /* 垂直前廊 */ 

/* 如下几个参数，自动计算 */ 
#define SSD_HT          (SSD_HOR_RESOLUTION + SSD_HOR_BACK_PORCH + SSD_HOR_FRONT_PORCH)
#define SSD_HPS         (SSD_HOR_BACK_PORCH)
#define SSD_VT          (SSD_VER_RESOLUTION + SSD_VER_BACK_PORCH + SSD_VER_FRONT_PORCH)
#define SSD_VPS         (SSD_VER_BACK_PORCH)
   
/******************************************************************************************/
/* 函数申明 */

void lcd_wr_data(volatile uint16_t data);                                                    /* LCD写数据 */
void lcd_wr_regno(volatile uint16_t regno);                                                  /* LCD写寄存器编号/地址 */
void lcd_write_reg(uint16_t regno, uint16_t data);                                           /* LCD写寄存器的值 */

void lcd_init(void);                                                                         /* 初始化LCD */ 
void lcd_display_on(void);                                                                   /* 开显示 */ 
void lcd_display_off(void);                                                                  /* 关显示 */
void lcd_scan_dir(uint8_t dir);                                                              /* 设置屏扫描方向 */ 
void lcd_display_dir(uint8_t dir);                                                           /* 设置屏幕显示方向 */ 
void lcd_ssd_backlight_set(uint8_t pwm);                                                     /* SSD1963 背光控制 */ 

void lcd_write_ram_prepare(void);                                                            /* 准备写GRAM */ 
void lcd_set_cursor(uint16_t x, uint16_t y);                                                 /* 设置光标 */ 
uint32_t lcd_read_point(uint16_t x, uint16_t y);                                             /* 读点(32位颜色,兼容LTDC) */
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color);                                 /* 画点(32位颜色,兼容LTDC) */

void lcd_clear(uint16_t color);                                                              /* LCD清屏 */
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);                    /* 填充实心圆 */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);                   /* 画圆 */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);                   /* 画水平线 */
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);              /* 设置窗口 */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);           /* 纯色填充矩形(32位颜色,兼容LTDC) */
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);    /* 彩色填充矩形 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);      /* 画直线 */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color); /* 画矩形 */

void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color);
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color);

#endif




