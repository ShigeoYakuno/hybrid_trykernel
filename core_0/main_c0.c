#include <stdio.h>
#include "pico/stdlib.h"
#include <hardware/flash.h>
#include "hardware/sync.h"
#include "pico/multicore.h"
#include "../core_1/application/hyb_public.h"
#include "../core_1/application/my_stdemb.h"
#include "core_0/lcd_1602_i2c_c0.h"

#define LED_PIN     16              // LEDのGPIO番号

// W25Q16JVの最終ブロック(Block31)のセクタ0の先頭アドレス = 0x1F0000
const uint32_t FLASH_TARGET_OFFSET = 0x1F0000;

uint8_t f_rd[7]={0};



void save_setting(void)
{
    // W25Q16JVの書き込み最小単 = FLASH_PAGE_SIZE(256Byte)
    uint8_t wd[FLASH_PAGE_SIZE] ={0};

    wd[0] = 'S';
    wd[1] = 'A';
    wd[2] = 'M';
    wd[3] = 'P';
    wd[4] = 'L';
    wd[5] = 'E';

    // 割り込み無効にする
    uint32_t ints = save_and_disable_interrupts();

    // コア0を停止
    multicore_lockout_start_blocking();

    // Flash消去
    //  消去単位　FLASH_SECTOR_SIZE(4096Byte) の倍数
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    // Flash書き込み
    //  書込単位　FLASH_PAGE_SIZE(256Byte) の倍数
    flash_range_program(FLASH_TARGET_OFFSET, wd, FLASH_PAGE_SIZE);

    // コア0を再開
    multicore_lockout_end_blocking();
    
    // 割り込みフラグを戻す
    restore_interrupts(ints);

    syslog(LOG_INFO,"write data: %s", wd);
}

void load_setting(void)
{
    // XIP_BASE(0x10000000)はflash.hで定義済み
    const uint8_t *f_block_rd = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
    
    f_rd[0] = f_block_rd[0];
    f_rd[1] = f_block_rd[1];
    f_rd[2] = f_block_rd[2];
    f_rd[3] = f_block_rd[3];
    f_rd[4] = f_block_rd[4];
    f_rd[5] = f_block_rd[5];

    syslog(LOG_INFO,"Read data: %s", f_rd);
}

void main_c0(void)
{
    /*c0は停止可能なサブコアとして定義*/
    multicore_lockout_victim_init();
    
    gpio_init(LED_PIN);                 // GPIO初期化
    gpio_set_dir(LED_PIN, GPIO_OUT);    // GPIOを出力に設定

    i2cLcdPerfInit();

    while (1) {
        gpio_put(LED_PIN, 1);           // LED点灯
        sleep_ms(500);                  // 500ミリ秒間休止
        gpio_put(LED_PIN, 0);           // LED消灯
        sleep_ms(500);                  // 500ミリ秒間休止
    }
}
