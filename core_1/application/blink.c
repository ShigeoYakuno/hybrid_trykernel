#include <trykernel.h>
#include "irq_sw.h"
#include "hyb_public.h"

#define LED_PIN     15              // LEDのGPIO番号
/*入力ピンアサイン*/
#define GPIO_SW1 16
#define GPIO_SW2 17
#define GPIO_SW3 19

#define GPIO_SW4 18
#define GPIO_SW5 20

#define GPIO_DRDY 21

#define GPIO16_EDGE_LOW    2
#define GPIO17_EDGE_LOW    6
#define GPIO18_EDGE_LOW    10
#define GPIO19_EDGE_LOW    14
#define GPIO20_EDGE_LOW    18
#define GPIO21_EDGE_LOW    22


void irq_bank0_inthdr(void);   

T_DINT  dint_irq  = {                       
    .intatr = TA_HLNG,
    .inthdr = irq_bank0_inthdr,
};

/* ======================================================================
			外部端子割り込みは全てIRQ_BANK0に取り込まれる
            処理の中で、どの端子からの割り込みかを判断する
   ====================================================================== */
void irq_bank0_inthdr(void)
{
    UW  val;

    /*割込み要因の取得*/
    for(int i = 0; i <4; i++) {
        val = in_w(INTR(i));

        /*登録したIRQの処理を追加する*/                                
        if(i == 2) {
            /*ADC DRDY*/
            if(val&(1<<GPIO21_EDGE_LOW)){            
                //adcDataRdy();
            }
            /*SW1*/
            if(val&(1<<GPIO16_EDGE_LOW)){            
                swEventPush(1);
            }
            /*SW2*/
            if(val&(1<<GPIO17_EDGE_LOW)){            
                swEventPush(2);
            }
            /*SW3*/
            if(val&(1<<GPIO19_EDGE_LOW)){            
                swEventPush(3);
            }
            /*SW4*/
            if(val&(1<<GPIO18_EDGE_LOW)){            
                swEventPush(4);
            }
            /*SW5*/
            if(val&(1<<GPIO20_EDGE_LOW)){            
                swEventPush(5);
            }
        }
        out_w(INTR(i), val);    /*割込み要因のクリア*/
    }
}

void irqIntInit(void)
{
    /*IRQ フラグのクリアと許可*/
    for(int i = 0; i <3; i++) out_w(INTR(i), in_w(INTR(i)));    
    ClearInt(IRQ_BANK0);
    EnableInt(IRQ_BANK0, 2); 
}
   
   /* ======================================================================
       IRQのピン設定
      ====================================================================== */
static void irqPerifSet(void)
{
    /*DRDY*/
    gpio_enable_input(GPIO_DRDY);                         
    gpio_set_intmode(GPIO_DRDY, INTE_MODE_EDGE_LOW );
    gpio_enable_pullup(GPIO_DRDY);

    /*SW1*/
    gpio_enable_input(GPIO_SW1);                         
    gpio_set_intmode(GPIO_SW1, INTE_MODE_EDGE_LOW );
    gpio_enable_pullup(GPIO_SW1);

    /*SW2*/
    gpio_enable_input(GPIO_SW2);                         
    gpio_set_intmode(GPIO_SW2, INTE_MODE_EDGE_LOW );
    gpio_enable_pullup(GPIO_SW2);

    /*SW3*/
    gpio_enable_input(GPIO_SW3);                         
    gpio_set_intmode(GPIO_SW3, INTE_MODE_EDGE_LOW );
    gpio_enable_pullup(GPIO_SW3);

    /*SW4*/
    gpio_enable_input(GPIO_SW4);                         
    gpio_set_intmode(GPIO_SW4, INTE_MODE_EDGE_LOW );
    gpio_enable_pullup(GPIO_SW4);

    /*SW5*/
    gpio_enable_input(GPIO_SW5);                         
    gpio_set_intmode(GPIO_SW5, INTE_MODE_EDGE_LOW );
    gpio_enable_pullup(GPIO_SW5);

}


/* ======================================================================
    10マイクロ秒待ちユーザ関数
    この時間占有する。タスク独立部(割込ハンドラ)から呼びすぎないこと
   ====================================================================== */
void wait_10micro(void)
{
    UW      t0, t;
    t0 = in_w(TIMER_TIMELR);
    do {
        t = in_w(TIMER_TIMELR);
    } while(t-t0 <= 10);
}
   

int usermain(void)
{
    /* LEDのポートの初期化 */
    gpio_enable_output(LED_PIN,0);

    /*IRQハンドラ登録*/
    tk_def_int(IRQ_BANK0, &dint_irq);
    tk_dly_tsk(100);
    /* IRQ設定 */
    irqPerifSet();
    
    /*SWタスクの起動*/
    initSWTask();

    tk_dly_tsk(100);
    /*ここよりIRQ許可*/
    irqIntInit();

    /* LEDの点滅 */
    while(1) {
        out_w(GPIO_OUT_XOR, 1<<LED_PIN);    // LED反転
        tk_dly_tsk(500);                    // 500ミリ秒休止
    }
    return 0;
}
