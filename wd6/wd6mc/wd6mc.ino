//wd6 arduino

#include "vnh5019.h"
#include "wd6re.h"
#include "wd6md.h"
#include "sh1tmr.h"
#include "uccbcrc.h"
#include "RunningAverage.h"

int wd6re_readrpm(WD6MD *wd6md);
int wd6md_setrpm(WD6MD *wd6md, int rpm, int dir);
int  wd6md_setspeed1(int16_t ms, int16_t *ms_p, WD6MD *wd6md1, WD6MD *wd6md2, WD6MD *wd6md3);
uint16_t qe_rpm_tbi(WD6RE *wd6re);

VNH5019MD g_md_J1(47,48,6,42,43,A1,&OCR4A,0,wd6re_isrJ1);
VNH5019MD g_md_J2(24,25,7,22,23,A2,&OCR4B,1,wd6re_isrJ2);
VNH5019MD g_md_J3(28,29,8,26,27,A3,&OCR4C,5,wd6re_isrJ3);
VNH5019MD g_md_B1(32,33,44,30,31,A4,&OCR5C,4,wd6re_isrB1);
VNH5019MD g_md_B2(36,37,45,34,35,A5,&OCR5B,3,wd6re_isrB2);
VNH5019MD g_md_B3(40,41,46,38,39,A6,&OCR5A,2,wd6re_isrB3);

WD6RE g_wd6re_J1={0};
WD6RE g_wd6re_J2={0};
WD6RE g_wd6re_J3={0};
WD6RE g_wd6re_B1={0};
WD6RE g_wd6re_B2={0};
WD6RE g_wd6re_B3={0};

WD6MD g_wd6md_J1={0};
WD6MD g_wd6md_J2={0};
WD6MD g_wd6md_J3={0};
WD6MD g_wd6md_B1={0};
WD6MD g_wd6md_B2={0};
WD6MD g_wd6md_B3={0};

RunningAverage g_wd6md_J1_mc(5);
RunningAverage g_wd6md_J2_mc(5);
RunningAverage g_wd6md_J3_mc(5);
RunningAverage g_wd6md_B1_mc(5);
RunningAverage g_wd6md_B2_mc(5);
RunningAverage g_wd6md_B3_mc(5);

//general
unsigned long g_millis=0;
unsigned long g_loop_cnt=0;         //loop counter
unsigned long g_loop_cps=0;         //loop counter per sec
unsigned long g_loop_ct=0;

MYTMR g_tmr_comm={0};
MYTMR g_tmr_rmc={0};        //read motor current

int g_force_send=0;

unsigned char g_rb=0;
unsigned long g_chmst=0;    //change motor speed time
int g_msn=0,g_mso=0;

unsigned long g_wcu_commpkt_counter=0;

//battery
int16_t g_cb_battV=-1;

//joy
int16_t g_cb_tsX=-1;
int16_t g_cb_tsY=-1;

int16_t g_cb_fsX=-1;
int16_t g_cb_fsY=-1;
int16_t g_cb_fsZ=-1;
int16_t g_cb_fsBS=-1;
int16_t g_cb_fsBE=-1;

int16_t g_cb_b6pBS=-1;
int16_t g_cb_b6pBE=-1;

int16_t g_cb_m1s=0;
int16_t g_cb_m2s=0;
int16_t g_cb_rdd=0;

int8_t g_wd6md_am=0;
int8_t g_wd6md_am_go=0;
int g_wd6md_am_goidx=-1;
