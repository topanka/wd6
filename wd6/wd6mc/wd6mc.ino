//wd6 arduino

#include "vnh5019.h"
#include "wd6re.h"
#include "wd6md.h"
#include "sh1tmr.h"
#include "uccbcrc.h"

VNH5019MD g_md_J1(0,0,0,0,0,0,&OCR4A,0,wd6re_isrJ1);
VNH5019MD g_md_J2(24,25,7,22,23,A2,&OCR4B,1,wd6re_isrJ2);
VNH5019MD g_md_J3(28,29,8,26,27,A3,&OCR4C,5,wd6re_isrJ3);
VNH5019MD g_md_B1(0,0,0,44,0,0,&OCR5C,4,wd6re_isrB1);
VNH5019MD g_md_B2(0,0,0,45,0,0,&OCR5B,3,wd6re_isrB2);
VNH5019MD g_md_B3(40,41,46,38,39,A6,&OCR5A,2,wd6re_isrB3);

WD6RE g_wd6re_J1={0};
WD6RE g_wd6re_J2={0};
WD6RE g_wd6re_J3={0};
WD6RE g_wd6re_B1={0};
WD6RE g_wd6re_B2={0};
WD6RE g_wd6re_B3={0};

WD6MD g_wd6mdJ1={0};
WD6MD g_wd6mdJ2={0};
WD6MD g_wd6mdJ3={0};
WD6MD g_wd6mdB1={0};
WD6MD g_wd6mdB2={0};
WD6MD g_wd6mdB3={0};

//general
unsigned long g_millis=0;
unsigned long g_loop_cnt=0;         //loop counter
unsigned long g_loop_cps=0;         //loop counter per sec
unsigned long g_loop_ct=0;

MYTMR g_tmr_comm={0};

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