

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "core_0/lcd_1602_i2c_c0.h"




/* ======================================================================
		ACM1602用　LCDへの文字送信
        引数：uint8_t data：送信する文字
        文字列を送信する場合は、lcdPrintを使う
   ====================================================================== */
static void i2cLcdWriteData(uint8_t data)
{
    uint8_t t_data[2] = { LCD_DATA, 0x00 };

    t_data[1] = data;
    i2c_write_blocking(i2c_default, LCD_DEV_ADD, t_data, sizeof(t_data), false);
    sleep_ms(5);
        
}

/* ======================================================================
		ACM1602用　LCDへの文字列送信
        引数：char str：送信する文字
        lcdSetCursorで表示場所を決めない場合、現在のカーソル位置から描画
   ====================================================================== */
void lcdPrint(char* str)
{
    while (*str) {
        i2cLcdWriteData((uint8_t)*str++);
    }
}


/* ======================================================================
		ACM1602用　LCDへのコマンド送信
        引数：char t_command：送信するコマンド
   ====================================================================== */

void i2clcd_writeCommand(char t_command){
    char t_data[2];
    t_data[0]=LCD_CMD;
    t_data[1]=t_command;
    i2c_write_blocking(i2c_default, LCD_DEV_ADD, t_data, sizeof(t_data), false);
    sleep_ms(5);
}

/* ======================================================================
    LCD表示クリア
   ====================================================================== */

void lcd_clear(void) {
    i2clcd_writeCommand( ACM_CMD_CLEAR);
}

/* ======================================================================
		ACM1602用　LCDのカーソル位置設定
        引数：col：横の位置、row：縦の位置(1段目、2段目)
   ====================================================================== */
   static void lcdSetCursor(uint8_t col,uint8_t row)
   {
        uint8_t row_offsets[] = { 0x00, 0x40};
       
       if (row > 1) {		//2段目以上はない。
           row = 1;
       }
       i2clcd_writeCommand( ACM_CMD_CUR | (col + row_offsets[row]));
   }


/* ======================================================================
		ACM1602用　初期設定
   ====================================================================== */
void lcdSettingACM1602(void)
{

    /*画面クリア*/
    i2clcd_writeCommand( ACM_CMD_CLEAR);
    /*ファンクション設定*/
    i2clcd_writeCommand( ACM_CMD_FUNC|DATA_8BIT|LINE_2);
    /*ディスプレイＯＮ、CURSOR-ON、blinking-ON*/
    i2clcd_writeCommand( ACM_CMD_DISP|DISP_ON|CURSOR_ON|BLINK_CUR);
    /*データ書き込み後アドレス加算モード設定*/
    i2clcd_writeCommand( ACM_CMD_ENTRY|SHIFT_ENA);
}

/* ======================================================================
    LCD表示テスト
   ====================================================================== */

void lcdInitDisp(void)
{
    lcdSetCursor(0, 0);		//カーソル位置1行目の左端
    lcdPrint("POCO_SDK + ITRON");
    lcdSetCursor(0, 1);		//カーソル位置2行目の左端
    lcdPrint("HYB SYS START!");
}
   
/* ======================================================================
		I2C接続LCDのペリフェラル初期化
   ====================================================================== */

int i2cLcdPerfInit(void) 
{
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c/lcd_1602_i2c example requires a board with I2C pins
#else
    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 50 * 1000);  //I2C初期化（50kHz）
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    //gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    //gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    lcdSettingACM1602();

    lcdInitDisp();

#endif
}
