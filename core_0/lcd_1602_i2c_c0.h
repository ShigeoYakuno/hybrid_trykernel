#ifndef I2C_LCD_H
#define I2C_LCD_H


#define LCD_DEV_ADD   0x50   //ACM1602
//#define LCD_DEV_ADD   0x27    //SCM1602    
#define LCD_CMD       0x00	 //コマンド送信
#define	LCD_DATA      0x80	 //データ送信

#define ACM_CMD_CLEAR 0x01
#define ACM_CMD_RETUN 0x02

#define ACM_CMD_ENTRY 0x04
#define SHIFT_ENA     0x02

#define ACM_CMD_DISP  0x08
#define DISP_ON       0x04 
#define CURSOR_ON     0x02 
#define BLINK_CUR     0x01 

#define ACM_CMD_SHIFT 0x10

#define ACM_CMD_FUNC  0x20
#define DATA_8BIT     0x10
#define DATA_4BIT     0x00 
#define LINE_2        0x08
#define LINE_1        0x00
#define DOT_5X10      0x04
#define DOT_5X8       0x00

#define ACM_CMD_CUR  0x80

int i2cLcdPerfInit(void);

#endif /* I2C_LCD_H */

