#include <trykernel.h>
#include "irq_sw.h"
#include "my_stdemb.h"
#include "hyb_public.h"


#define FLGPTN_SW_EVENT 0x0000001F

#define FLGPTN_SW1_PUSH 0x00000001
#define FLGPTN_SW2_PUSH 0x00000002
#define FLGPTN_SW3_PUSH 0x00000004
#define FLGPTN_SW4_PUSH 0x00000008
#define FLGPTN_SW5_PUSH 0x00000010

#define SW_MAX 5


/* -------------------------------------------------------------------------
            構造体定義
    ----------------------------------------------------------------------*/

typedef struct {
    ID      flgid;              // イベントフラグID
    UB      sw_state[SW_MAX];   // SW状態 0か1
} T_SW_DCB;    
static T_SW_DCB sw_cb;


/* -------------------------------------------------------------------------
            内部変数定義
    ----------------------------------------------------------------------*/
/* -------------------------------------------------------------------------
            外部変数定義
    ----------------------------------------------------------------------*/	
/* -------------------------------------------------------------------------
            外部変数参照
    ----------------------------------------------------------------------*/
/* -------------------------------------------------------------------------
            プロトタイプ宣言
    ----------------------------------------------------------------------*/
/* -------------------------------------------------------------------------
            プロトタイプ宣言(内部)
    ----------------------------------------------------------------------*/
static void tsk_sw(INT stacd, void *exinf);
/* -------------------------------------------------------------------------
            ＯＳ定義
    ----------------------------------------------------------------------*/

ID  tskid_sw;                          // タスクID番号
#define STKSZ_SW   1024                // スタックサイズ
UW  tskstk_sw[STKSZ_SW/sizeof(UW)];   // スタック領域

/* LCDタスク生成情報 */
T_CTSK ctsk_sw = {
    .tskatr     = TA_HLNG | TA_RNG3 | TA_USERBUF,
    .task       = tsk_sw,
    .stksz      = STKSZ_SW,
    .itskpri    = 8,
    .bufptr     = tskstk_sw,
};


/* ======================================================================
        SW1~5の割り込み処理
   ====================================================================== */
void swEventPush(UB no)
{
    switch(no){
        case 1: tk_set_flg(getSwFlgID(), FLGPTN_SW1_PUSH); break;
        case 2: tk_set_flg(getSwFlgID(), FLGPTN_SW2_PUSH); break;
        case 3: tk_set_flg(getSwFlgID(), FLGPTN_SW3_PUSH); break;
        case 4: tk_set_flg(getSwFlgID(), FLGPTN_SW4_PUSH); break;
        case 5: tk_set_flg(getSwFlgID(), FLGPTN_SW5_PUSH); break;
        default:break;
    }
    
}


/* ======================================================================
		LCD　タスク起動
   ====================================================================== */
void initSWTask(void)
{
    tskid_sw = tk_cre_tsk(&ctsk_sw);
    tk_sta_tsk(tskid_sw, 0);
}


ID getSwFlgID(void)
{
    return sw_cb.flgid;
}

/* ======================================================================
		SWタスク　メインルーチン
   ====================================================================== */
static void tsk_sw(INT stacd, void *exinf)
{

    T_CFLG  cflg = {
        .flgatr     = TA_TFIFO | TA_WMUL,
        .iflgptn    = 0,
    };
    UINT    flgptn;
    ER  err;
    T_RFLG	rflg;

    /* DRDY割り込みからのイベントフラグ生成 */
    err = tk_cre_flg(&cflg);

    if(err < E_OK){
        syslog(LOG_INFO,"sw_flag err");
    }
    else{
        syslog(LOG_INFO,"SW TASK START! %d",err);
        sw_cb.flgid = err;
    }


    while(1) {

        tk_wai_flg(getSwFlgID(), FLGPTN_SW_EVENT, TWF_ORW , &flgptn, TMO_FEVR);

        tk_ref_flg(getSwFlgID(),&rflg);

        syslog(LOG_INFO,"sw ptn = %d",rflg.flgptn);

        tk_dly_tsk(250);    /*チャタリング簡易*/

        /*ゼロ*/
        if( rflg.flgptn & FLGPTN_SW1_PUSH  ){
            //Cmd_ZExec();
            save_setting();
			tk_clr_flg( getSwFlgID(), ~FLGPTN_SW1_PUSH );
        }
        /*スパン*/
        if( rflg.flgptn & FLGPTN_SW2_PUSH  ){
            //Cmd_TExec();
            load_setting();
			tk_clr_flg( getSwFlgID(), ~FLGPTN_SW2_PUSH );
        }
        /*記憶*/
        if( rflg.flgptn & FLGPTN_SW3_PUSH  ){
			tk_clr_flg( getSwFlgID(), ~FLGPTN_SW3_PUSH );
        }
        if( rflg.flgptn & FLGPTN_SW4_PUSH  ){
			tk_clr_flg( getSwFlgID(), ~FLGPTN_SW4_PUSH );
        }
        if( rflg.flgptn & FLGPTN_SW5_PUSH  ){
			tk_clr_flg( getSwFlgID(), ~FLGPTN_SW5_PUSH );
        }

    }
    tk_slp_tsk(TMO_FEVR);
}
