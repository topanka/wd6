//#include <Servo.h> 

#include <wd6st.h>
#include <sh1tmr.h>
#include <uccbcrc.h>

#include <RunningAverage.h>

#define WD6CU_VERSION        "1.0.2"

//battery
#define UCCB_BATTV_PORT              A16
#define UCCB_BATTA_PORT              A17
int16_t g_battV=-1;
int16_t g_battA=-1;

//temperature
#define UCCB_TEMPERATURE_PORT       A4
int16_t g_temperature=-1;
MYTMR g_tmr_temperature={0};

//general
unsigned long g_millis=0;
unsigned long g_loop_cnt=0;         //loop counter
unsigned long g_loop_cps=0;         //loop counter per sec
unsigned long g_loop_ct=0;
int g_recv_ready=0;                 //uccb command received

//UCCB

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

//10p switch
int g_cb_sw10p=-1;

//6p button
int g_cb_b6p=-1;
int16_t g_cb_b6pBS=-1;
int16_t g_cb_b6pBE=-1;

//comm
unsigned long g_cb_w_commpkt_counter=0;

//servo
int16_t g_cb_tsxp=0;
int16_t g_cb_tsyp=0;

int16_t g_cb_rdd=0;

//lights

#define SH1_POSLIGHT_PORT        26
int g_cb_lightpos=UCCB_PL_OFF;
MYTMR g_tmr_lightpos={0};

//motor
int16_t g_cb_m1s=0;
int16_t g_cb_m2s=0;

uint16_t g_rpm_m1=0;
uint16_t g_rpm_m2=0;
uint16_t g_cur_m1=0;
uint16_t g_cur_m2=0;
int8_t g_dir_m1=0;
int8_t g_dir_m2=0;

MYTMR g_tmr_checkmc={0};

unsigned long g_mc_loopcps=0;

uint16_t g_mc_J1rpm=0;
uint16_t g_mc_B1rpm=0;
uint16_t g_mc_J2rpm=0;
uint16_t g_mc_B2rpm=0;
uint16_t g_mc_J3rpm=0;
uint16_t g_mc_B3rpm=0;
uint16_t g_mc_J1cur=0;
uint16_t g_mc_B1cur=0;
uint16_t g_mc_J2cur=0;
uint16_t g_mc_B2cur=0;
uint16_t g_mc_J3cur=0;
uint16_t g_mc_B3cur=0;
